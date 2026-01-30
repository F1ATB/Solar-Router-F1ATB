#include <vector>
#include <SPI.h>
#include <cstdint>
#define LIGHT_ADC 34
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <EcranLCD.h>
#include "CST820.h"

LGFX* lcd = nullptr;
CST820* touch = nullptr;  //Touch pour les écrans Capacitifs
unsigned long runtime_0 = 0, runtime_click = 0, runtime_On = 0;
int8_t NumPage = 0, NbrPage = 7;
int16_t ClickX = 0, ClickY = 0, ClickCount = 0, LastClickX = 0, LastClickY = 0, slideX = 0;
int LigneTotalOld, LigneTotal, LigneIdx;
bool ReDraw = false, ScreenOn = true;
int16_t ForceIPidx = 0, ForceIdx = 0, ForceOnOff = 0;
int16_t idxDecalM = 0;


unsigned long CoulTexte, CoulFond, CoulBouton, CoulBoutFond, CoulBoutBord, CoulW, CoulWh, CoulTabTexte, CoulTabFond, CoulTabBord;
unsigned long CoulSaisieTexte, CoulSaisieFond, CoulSaisieBord, CoulTemp, CoulGrTexte, CoulGrFond;

void Ecran_Init(byte ESP32type) {
  pinMode(4, OUTPUT);    //LED rouge
  pinMode(16, OUTPUT);   //LED bleue ou verte
  pinMode(17, OUTPUT);   //LED bleue ou verte
  digitalWrite(4, 1);    //Extinction
  digitalWrite(16, 1);   //Extinction
  digitalWrite(17, 1);   //Extinction
  pinMode(35, INPUT);    //Entrée capteur Infra-Rouge
  if (ESP32type >= 8  ) {  //Capacitive
    delay(100);
    touch = new CST820(I2C_SDA, I2C_SCL, TP_RST, TP_INT);
    touch->begin();
  }
  lcd = new LGFX(ESP32type);
  lcd->init();
  // Réinitialiser le mode non miroir
  lcd->writecommand(0x36);  // Commande MADCTL
  lcd->writedata(0x00);     // Mode normal sans inversion
  int R=rotation;
  if (ESP32_Type ==9) R=(R+2)%4;
  lcd->setRotation(R);
  SetCouleurs();
  lcd->setFont(&fonts::AsciiFont8x16);
  if (Calibre[7] != 0 && ESP32type < 8) {
    lcd->setTouchCalibrate(Calibre);  //Ancienne calibration ecran
  }
  NumPage=NumPageBoot;
  ScreenOn=true;
  ReDraw=true;
  TraceMessages();
}
void Ecran_Loop() {
  //***************************************
  //Pages nécessitant un rafraichissement
  //***************************************
  if (ReDraw && ScreenOn) GoPage(NumPage);
  if ((millis() - runtime_0) > 3000) {
    if (ScreenOn) {
      lcd->setBrightness(255);
      lcd->setTextColor(CoulTexte, CoulFond);
      switch (NumPage) {
        case 0:
          AccueilLoop();
          break;
        case 1:
          GrapheTrace(10);
          break;
        case 3:
          runtime_On = millis();  //Cas des messages de debug, on laisse allumé
          TraceMessages();
          break;
        case 4:
          TraceReseau();
          break;
        case 5:
          TraceWcolor();
          break;
        case 6:
          TraceGaugeW();
          break;
      }
    }

    runtime_0 = millis();
  }
  //***************************************
  // Gestion durée d'allumage de l'écran
  //***************************************
  if ((millis() - runtime_On) > DurEcran && DurEcran > 0) {
    lcd->clear(TFT_BLACK);
    lcd->setBrightness(0);
    ReDraw = true;
    ScreenOn = false;
    runtime_On = millis();
  }
  if (clickPresence == 1) {
    if (digitalRead(35)) {
      ScreenOn = true;
      lcd->setBrightness(255);
      runtime_On = millis();
    }
  }

  //***************************************
  // Gestion des clicks sur l'écran
  //***************************************
  if (millis() - runtime_click > 100) {  //Anti-glitch
    bool touched = false;
    runtime_click = millis();
    if (ESP32_Type >= 8) {  //Capacitif
      uint8_t gesture;
      uint16_t touchX, touchY, temp;
      if (touch->getTouch(&touchX, &touchY, &gesture)) {
        switch (rotation) {
          case 0:
            touchX = 239 - touchX;
            touchY = 319 - touchY;
            break;
          case 1:
            temp = touchX;
            touchX = 319 - touchY;
            touchY = temp;
            break;
          case 3:
            temp = touchX;
            touchX = touchY;
            touchY = 239 - temp;
            break;
        }
        touched = true;
        ClickX = (int16_t)touchX;
        ClickY = (int16_t)touchY;
      }

    } else {  //Resistif
      if (lcd->getTouch(&ClickX, &ClickY)) {
        touched = true;
      }
    }
    //L'écran a été touché
    if (touched) {
      lcd->setBrightness(255);
      runtime_On = millis();
      if (!ScreenOn) {  //On allume écran éteint
        ScreenOn = true;
        GoPage(NumPage);
      } else {  //L'écran est déjà allumé

        ClickCount++;                                           // Suite de click si on reste appuyé
        if (ClickCount > 15 && ESP32_Type <8) TraceCalibr();  //Pression de 3s force la calibration des resistifs
        if (Calibre[7] == 0 && ESP32_Type <8) {               //Pas encore calibré
          TraceCalibr();
        } else {
          if (ClickCount > 1 && abs(ClickY - LastClickY) > 30) {  // Déplacement vertical du doigt
            if (ClickY < LastClickY) {
              LigneIdx++;
            } else {
              LigneIdx--;
            }
            ClickCount = 0;
            ReDraw = true;
          }
          if (ClickCount > 1 && abs(ClickX - LastClickX) > 30) {  // Déplacement horizontal du doigt
            if (ClickX < LastClickX) {
              NumPage = (NumPage + 1 + NbrPage) % NbrPage;
              slideX = 40 - lcd->width();  //Masque par la droite
            } else {
              NumPage = (NumPage - 1 + NbrPage) % NbrPage;
              slideX = 40;
            }
            ClickCount = 0;
            runtime_0 = millis();  //Pour arreter le rafraichissement
          } else {                 // Gestion des clicks sur certaines pages. Doigt fixe.
            switch (NumPage) {
              case 0:
                if (ClickCount > 5) AccueilClick();
                break;
              case 10:
                AccueilForceClick();
                break;
            }
          }
        }
      }
      LastClickX = ClickX;
      LastClickY = ClickY;
    } else {
      ClickCount = 0;
    }
    if (slideX != 0) {  // slide page en cours
      slideX = (slideX + 40) % lcd->width();
      if (slideX == 0) {
        GoPage(NumPage);
        runtime_0 = millis() - 5000;
      } else if (slideX < 0) {
        lcd->fillRect(-slideX, 0, lcd->width() + slideX, lcd->height(), CoulFond);
        lcd->fillRect(-slideX, 0, 2, lcd->height(), TFT_BLACK);
      } else {
        lcd->fillRect(0, 0, slideX, lcd->height(), CoulFond);
        lcd->fillRect(slideX - 2, 0, 2, lcd->height(), TFT_BLACK);
      }
    }
  }
}
//*********************
// Gestion des couleurs
//*********************
void SetCouleurs() {
  if (Couleurs.length() > 6) {
    CoulTexte = ConvCouleur(Couleurs.substring(0, 6));
    CoulFond = ConvCouleur(Couleurs.substring(6, 12));
    CoulBouton = ConvCouleur(Couleurs.substring(18, 24));
    CoulBoutFond = ConvCouleur(Couleurs.substring(24, 30));
    CoulBoutBord = ConvCouleur(Couleurs.substring(30, 36));
    CoulSaisieTexte = ConvCouleur(Couleurs.substring(36, 42));
    CoulSaisieFond = ConvCouleur(Couleurs.substring(42, 48));
    CoulSaisieBord = ConvCouleur(Couleurs.substring(48, 54));
    CoulTabTexte = ConvCouleur(Couleurs.substring(54, 60));
    CoulTabFond = ConvCouleur(Couleurs.substring(60, 66));
    CoulTabBord = ConvCouleur(Couleurs.substring(66, 72));
    CoulW = ConvCouleur(Couleurs.substring(72, 78));
    CoulWh = ConvCouleur(Couleurs.substring(84, 90));
    CoulGrTexte = ConvCouleur(Couleurs.substring(114, 120));
    CoulGrFond = ConvCouleur(Couleurs.substring(120, 126));
    CoulTemp = ConvCouleur(Couleurs.substring(132, 138));
    lcd->fillScreen(CoulFond);
    lcd->setTextColor(CoulTexte, CoulFond);
  }
}

unsigned long WattToColor() {
  unsigned long couleur = 0;
  int LastW = PuissanceS_M - PuissanceI_M;
  if (LastW >= 5000) {
    couleur = ConvCouleur("FF0000");
  } else if (LastW < 5000 && LastW >= 500) {
    couleur = calculCouleur(LastW, 500, 5000, 255, 140, 0, 255, 0, 0);
  } else if (LastW < 500 && LastW >= 0) {
    couleur = calculCouleur(LastW, 0, 500, 0, 255, 0, 255, 140, 0);
  } else if (LastW < 0 && LastW >= -500) {
    couleur = calculCouleur(LastW, -500, 0, 0, 0, 255, 0, 255, 0);
  } else if (LastW < -500 && LastW > -5000) {
    couleur = calculCouleur(LastW, -5000, -500, 0, 255, 255, 0, 0, 255);
  } else if (LastW <= -5000) {
    couleur = ConvCouleur("00FFFF");
  }
  return couleur;
}
unsigned long calculCouleur(int LastW, int S1, int S2, int r1, int v1, int b1, int r2, int v2, int b2) {  //Couleur interpollé / Watt
  float K = float(LastW - S1) / float(S2 - S1);
  int red = r1 + int(float(r2 - r1) * K);
  red = red & 0x0000FF;
  red = (red << 16);
  int green = v1 + int(float(v2 - v1) * K);
  green = green & 0x0000FF;
  green = (green << 8);
  int blue = b1 + int(float(b2 - b1) * K);
  blue = blue & 0x0000FF;
  red = red + green + blue;
  return (unsigned long)red;
}
//*********************************
// Affichage des différentes pages
//*********************************
void GoPage(int N) {
  if (ScreenOn) {
    ReDraw = false;
    NumPage = N;
    lcd->fillScreen(CoulFond);
    lcd->setTextColor(CoulTexte, CoulFond);
    switch (NumPage) {
      case 0:
        AccueilTrace();
        break;
      case 1:
        GrapheTrace(10);
        break;
      case 2:
        GrapheTrace(48);
        break;
      case 3:
        TraceMessages();
        break;
      case 4:
        TraceReseau();
        break;
      case 5:
        TraceWcolor();
        break;
      case 6:
        TraceGaugeW();
        break;
    }
  }
}

void AccueilTrace() {
  int WE, HE, W, H, Hconso, H1, H2, H3, Hact, Hdelta, Hbas;
  float F0, F1, F2;
  String NomR, ActionR, ActionsR, TempR, TempsR, valeur0, valeur;
  Liste_NomsEtats(0);  //On rafraichi interne
  AccueilCadrage(WE, HE, W, H, Hconso, H1, H2, H3, Hact, Hdelta, Hbas, F0, F1, F2);

  //Tableau consommations
  PrintCentre(Ascii("Soutirée"), W + W / 2, Hconso - 2, F0);
  PrintCentre(Ascii("Injectée"), 2 * W + W / 2, Hconso - 2, F0);
  lcd->fillRect(0, H1, 3 * W, H, CoulW);
  lcd->fillRect(0, H2, 3 * W, H, CoulWh);
  lcd->drawRect(0, H1, 3 * W, H, CoulTabBord);
  lcd->drawRect(0, H2, 3 * W, H, CoulTabBord);
  lcd->drawRect(W, Hconso, W, H2 + H - Hconso, CoulTabBord);
  lcd->drawRect(2 * W, Hconso, W, H2 + H - Hconso, CoulTabBord);
  lcd->setTextColor(CoulTabTexte, CoulW);
  PrintGauche("P (W)", 0, H1, F1);
  lcd->setTextColor(CoulTabTexte, CoulWh);
  PrintGauche("E (Wh)", 0, H2, F1);

  //Tableau Action puis température
  //Tableau actions en cours
  int LigneCount = 0;
  lcd->setTextColor(CoulTabTexte, CoulTabFond);
  for (int i = 0; i < LES_ROUTEURS_MAX; i++) {
    if (RMS_IP[i] > 0 && RMS_Note[i] > 0) {
      SplitS(RMS_NomEtat[i], NomR, US, ActionsR);
      SplitS(ActionsR, TempsR, US, ActionsR);
      while (ActionsR.length() > 1 && (Hact + H3) <= Hbas) {
        SplitS(ActionsR, ActionR, FS, ActionsR);
        if (LigneCount >= LigneIdx) {
          SplitS(ActionR, valeur, ES, ActionR);
          SplitS(ActionR, valeur0, ES, valeur);  //Nom
          lcd->fillRect(0, Hact, WE, H3, CoulTabFond);
          PrintGauche(Ascii(valeur0.substring(0, 16)), 0, Hact, F2);
          lcd->drawRect(0, Hact, 2 * W, H3, CoulTabBord);
          lcd->drawRect(2 * W, Hact, W, H3, CoulTabBord);
          lcd->drawRect(3 * W, Hact, W, H3, CoulTabBord);
          Hact += H3;
        }
        LigneCount++;
      }
    }
  }
  Hact += Hdelta;
  //Tableau Températures
  lcd->setTextColor(CoulTabTexte, CoulTemp);
  for (int i = 0; i < LES_ROUTEURS_MAX; i++) {
    if (RMS_IP[i] > 0 && RMS_Note[i] > 0) {
      SplitS(RMS_NomEtat[i], NomR, US, ActionsR);
      SplitS(ActionsR, TempsR, US, ActionsR);
      while (TempsR.length() > 1 && (Hact + H3) <= Hbas) {
        SplitS(TempsR, TempR, FS, TempsR);
        if (TempR.indexOf("tempInt") > 0) {
          if (LigneCount >= LigneIdx) {
            SplitS(TempR, valeur, ES, TempR);
            SplitS(TempR, valeur0, ES, valeur);  //Nom
            lcd->fillRect(0, Hact, WE, H3, CoulTemp);
            PrintGauche(Ascii(valeur0.substring(0, 24)), 0, Hact, F2);
            lcd->drawRect(0, Hact, 3 * W, H3, CoulTabBord);
            lcd->drawRect(3 * W, Hact, W, H3, CoulTabBord);
            Hact += H3;
          }
          LigneCount++;
        }
      }
    }
  }
  if (LigneIdx > 0) {  //Il y a des lignes avant
    lcd->fillTriangle(2 * W - 14, H2 + H, 2 * W - 2, H2 + H, 2 * W - 8, H2 + H + 6, CoulTabBord);
  }
  if (LigneCount < LigneTotal) {  //Il y a des lignes après
    lcd->fillTriangle(2 * W - 14, Hact, 2 * W - 2, Hact, 2 * W - 8, Hact - 6, CoulTabBord);
  }
}
void AccueilClick() {
  int WE, HE, W, H, Hconso, H1, H2, H3, Hact, Hdelta, Hbas;
  float F0, F1, F2;
  AccueilCadrage(WE, HE, W, H, Hconso, H1, H2, H3, Hact, Hdelta, Hbas, F0, F1, F2);

  String NomR, NomAct, ActionR, ActionsR, TempR, TempsR, valeur, valeur0;
  int LigneCount = 0;
  //Tableau actions en cours
  for (int i = 0; i < LES_ROUTEURS_MAX; i++) {
    if (RMS_IP[i] > 0 && RMS_Note[i] > 0) {
      SplitS(RMS_NomEtat[i], NomR, US, ActionsR);
      SplitS(ActionsR, TempsR, US, ActionsR);
      while (ActionsR.length() > 1 && (Hact + H3) <= Hbas) {
        SplitS(ActionsR, ActionR, FS, ActionsR);
        if (LigneCount >= LigneIdx) {
          if (ClickY >= Hact && ClickY <= (Hact + H3)) {
            SplitS(ActionR, valeur0, ES, ActionR);
            ForceIdx = valeur0.toInt();  //Num Action
            ForceIPidx = i;
            SplitS(ActionR, NomAct, ES, valeur);
            SplitS(valeur, valeur0, ES, valeur);  //Ouverture
            SplitS(valeur, valeur0, ES, valeur);  //Hequiv
            SplitS(valeur, valeur0, ES, valeur);  //tOnOff
            NumPage = 10;
            ClickCount = -5;
            lcd->setTextColor(CoulTabTexte, CoulTabFond);
            lcd->fillRect(0, 30, WE, HE - 30, CoulTabFond);
            lcd->drawRect(0, 30, WE, HE - 30, CoulTabBord);
            PrintCentre(Ascii("Forçage"), WE / 2, 40, 1.5);
            PrintCentre(Ascii(NomR), WE / 2, 80, 1.5);
            PrintCentre(Ascii(NomAct), WE / 2, 120, 1.5);
            PrintCentre(valeur + " mn", WE / 2, 165, 1.5);
            ForceOnOff = valeur.toInt();  //Duree
            uint16_t coul = CoulBoutFond;
            if (ForceOnOff > 0) coul = TFT_RED;
            lcd->fillRect(2, 160, WE / 4, 40, coul);
            lcd->drawRect(2, 160, WE / 4, 40, CoulBoutBord);
            lcd->setTextColor(CoulBouton, coul);
            PrintCentre("On", WE / 8 + 1, 165, 1.5);
            coul = CoulBoutFond;
            if (ForceOnOff < 0) coul = TFT_RED;
            lcd->fillRect(WE - 2 - WE / 4, 160, WE / 4, 40, coul);
            lcd->drawRect(WE - 2 - WE / 4, 160, WE / 4, 40, CoulBoutBord);
            lcd->setTextColor(CoulBouton, coul);
            PrintCentre("Off", WE - WE / 8 - 1, 165, 1.5);
            i = 100;
          }
          Hact += H3;
        }
        LigneCount++;
      }
    }
  }
}

void AccueilForceClick() {
  if (ClickY >= 160 && ClickY <= 210) {
    int WE = lcd->width();
    if (ClickX < WE / 2) {
      if (ForceOnOff < 0) {
        ForceOnOff = 0;
      } else {
        ForceOnOff += 30;
      }
    } else {
      if (ForceOnOff > 0) {
        ForceOnOff = 0;
      } else {
        ForceOnOff -= 30;
      }
    }
    lcd->setTextColor(CoulTabTexte, CoulTabFond);
    PrintCentre("        ", WE / 2, 165, 1.5);
    PrintCentre(String(ForceOnOff) + " mn", WE / 2, 165, 1.5);
    uint16_t coul = CoulBoutFond;
    if (ForceOnOff > 0) coul = TFT_RED;
    lcd->fillRect(2, 160, WE / 4, 40, coul);
    lcd->drawRect(2, 160, WE / 4, 40, CoulBoutBord);
    lcd->setTextColor(CoulBouton, coul);
    PrintCentre("On", WE / 8 + 1, 165, 1.5);
    coul = CoulBoutFond;
    if (ForceOnOff < 0) coul = TFT_RED;
    lcd->fillRect(WE - 2 - WE / 4, 160, WE / 4, 40, coul);
    lcd->drawRect(WE - 2 - WE / 4, 160, WE / 4, 40, CoulBoutBord);
    lcd->setTextColor(CoulBouton, coul);
    PrintCentre("Off", WE - WE / 8 - 1, 165, 1.5);
    if (ForceIPidx == 0) {
      LesActions[ForceIdx].tOnOff = ForceOnOff;
    } else {
      WiFiClient clientESP_RMS;
      String host = IP2String(RMS_IP[ForceIPidx]);
      if (!clientESP_RMS.connect(host.c_str(), 80)) {
        StockMessage("connection to ESP_RMS (Force)  : " + host + " failed");
        clientESP_RMS.stop();
        if (RMS_Note[ForceIPidx] > 0) RMS_Note[ForceIPidx]--;
        delay(2);
        return;
      }
      String url = "/ForceAction?Force=" + String(ForceOnOff) + "&NumAction=" + String(ForceIdx);
      clientESP_RMS.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
      unsigned long timeout = millis();
      while (clientESP_RMS.available() == 0) {
        if (millis() - timeout > 5000) {
          StockMessage("client ESP_RMS (Force) Timeout !" + host);
          clientESP_RMS.stop();
          RMS_Note[ForceIPidx] = false;
          return;
        }
      }
      timeout = millis();
      // Lecture des données brutes distantes
      while (clientESP_RMS.available() && (millis() - timeout < 5000)) {
        String Reponse = clientESP_RMS.readStringUntil('\r');
      }
      clientESP_RMS.stop();
    }
    RMS_Note[ForceIPidx] = true;
    Liste_NomsEtats(ForceIPidx);  //Pour rafraichir état
  } else {
    if (ClickCount > 2) GoPage(0);
  }
}

void AccueilLoop() {

  int WE, HE, W, H, Hconso, H1, H2, H3, Hact, Hdelta, Hbas;
  float F0, F1, F2;
  Liste_NomsEtats(0);  //On rafraichi interne
  AccueilCadrage(WE, HE, W, H, Hconso, H1, H2, H3, Hact, Hdelta, Hbas, F0, F1, F2);
  TraceDate();
  TraceTarif();
  lcd->fillRect(W + 1, H1 + 1, W - 2, H - 2, CoulW);
  lcd->fillRect(2 * W + 1, H1 + 1, W - 2, H - 2, CoulW);
  lcd->setTextColor(CoulTabTexte, CoulW);
  PrintDroite(String(PuissanceS_M), 2 * W, H1, F1);
  PrintDroite(String(PuissanceI_M), 3 * W, H1, F1);
  lcd->fillRect(W + 1, H2 + 1, W - 2, H - 2, CoulWh);
  lcd->fillRect(2 * W + 1, H2 + 1, W - 2, H - 2, CoulWh);
  lcd->setTextColor(CoulTabTexte, CoulWh);
  PrintDroite(String(EnergieJour_M_Soutiree), 2 * W, H2, F1);
  PrintDroite(String(EnergieJour_M_Injectee), 3 * W, H2, F1);
  uint16_t C = TFT_RED;
  float Angle = 0;
  if (PuissanceI_M > 0) {
    C = TFT_GREEN;
    Angle = float(PuissanceI_M) / 10;
  } else {
    Angle = float(PuissanceS_M) / 20;
  }
  Angle = min(float(360), Angle);
  Angle = Angle - 90;
  lcd->fillCircle(3.5 * W, H2 - 5, H + 1, TFT_WHITE);
  lcd->fillArc(3.5 * W, H2 - 5, H - 1, 0, -90, Angle, C);

  String NomR, ActionR, ActionsR, TempR, TempsR, valeur, valeur0;
  int LigneCount = 0;
  //Tableau actions en cours
  lcd->setTextColor(CoulTabTexte, CoulTabFond);

  for (int i = 0; i < LES_ROUTEURS_MAX; i++) {
    if (RMS_IP[i] > 0 && RMS_Note[i]) {
      SplitS(RMS_NomEtat[i], NomR, US, ActionsR);
      SplitS(ActionsR, TempsR, US, ActionsR);
      while (ActionsR.length() > 1 && (Hact + H3) <= Hbas) {
        SplitS(ActionsR, ActionR, FS, ActionsR);
        if (LigneCount >= LigneIdx) {
          SplitS(ActionR, valeur, ES, ActionR);
          SplitS(ActionR, valeur0, ES, valeur);
          SplitS(valeur, valeur0, ES, valeur);
          lcd->fillRect(2 * W + 1, Hact + 1, W - 2, H3 - 2, CoulTabFond);
          PrintDroite(valeur0 + "%", 3 * W, Hact, F2);  //Ouverture
          SplitS(valeur, valeur0, ES, valeur);
          int heure = int(valeur0.toInt() / 100);
          String mn = "00" + String(int((valeur0.toInt() - 100 * heure) * 0.6));
          mn = String(heure) + ":" + mn.substring(mn.length() - 2);
          lcd->fillRect(3 * W + 1, Hact + 1, W - 2, H3 - 2, CoulTabFond);
          PrintDroite(mn, WE, Hact, F2);
          Hact += H3;
        }
        LigneCount++;
      }
    }
  }
  Hact += Hdelta;
  //Tableau Températures
  lcd->setTextColor(CoulTabTexte, CoulTemp);
  for (int i = 0; i < LES_ROUTEURS_MAX; i++) {
    if (RMS_IP[i] > 0 && RMS_Note[i]) {
      SplitS(RMS_NomEtat[i], NomR, US, ActionsR);
      SplitS(ActionsR, TempsR, US, ActionsR);
      while (TempsR.length() > 1 && (Hact + H3) <= Hbas) {
        SplitS(TempsR, TempR, FS, TempsR);
        if (TempR.indexOf("tempInt") > 0) {
          if (LigneCount >= LigneIdx) {
            SplitS(TempR, valeur, ES, TempR);
            SplitS(TempR, valeur0, ES, valeur);
            SplitS(valeur, valeur, ES, valeur0);
            lcd->fillRect(3 * W + 1, Hact + 1, W - 2, H3 - 2, CoulTemp);
            PrintDroite(valeur + String(char(248)) + "C", WE, Hact, F2);
            Hact += H3;
          }
          LigneCount++;
        }
      }
    }
  }
}
void AccueilCadrage(int& WE, int& HE, int& W, int& H, int& Hconso, int& H1, int& H2, int& H3, int& Hact, int& Hdelta, int& Hbas, float& F0, float& F1, float& F2) {
  WE = lcd->width();
  HE = lcd->height();
  W = WE / 4;
  H = HE / (14 - 6 * float(rotation % 2));
  Hconso = 4;  //Haut tableau conso
  H1 = Hconso + H / 2 + 2;
  H2 = H1 + H;
  H3 = 20;
  F0 = 0.8 + 0.2 * float(rotation % 2);  //Fonte
  F1 = 1.2 + 0.3 * float(rotation % 2);
  LigneTotal = 0;

  String NomR, ActionR, ActionsR, TempR, TempsR;
  for (int i = 0; i < LES_ROUTEURS_MAX; i++) {
    if (RMS_IP[i] > 0 && RMS_Note[i]) {
      SplitS(RMS_NomEtat[i], NomR, US, ActionsR);
      SplitS(ActionsR, TempsR, US, ActionsR);
      while (ActionsR.length() > 1) {
        SplitS(ActionsR, ActionR, FS, ActionsR);
        LigneTotal++;
      }
      while (TempsR.length() > 1) {
        SplitS(TempsR, TempR, FS, TempsR);
        if (TempR.indexOf("tempInt") > 0) LigneTotal++;
      }
    }
  }

  Hbas = HE - 30 + 10 * float(rotation % 2);
  int espace = Hbas - (H2 + H);
  Hdelta = 0;
  F2 = 1;
  if (espace > LigneTotal * H3) {
    Hdelta = (espace - LigneTotal * H3) / 3;
    LigneIdx = 0;
  } else {
    LigneIdx = max(LigneIdx, 0);
    LigneIdx = min(LigneTotal, LigneIdx);
  }
  Hact = H2 + H + Hdelta;
  if (LigneTotal != LigneTotalOld) ReDraw = true;
  LigneTotalOld = LigneTotal;
}
void GrapheTrace(int8_t gr) {
  int WE = lcd->width();
  int HE = lcd->height();
  int delta;
  int Hm = HE / 2 + 10;
  unsigned long Cinject = ConvCouleur("00FFFF");
  int iS1, Y1, X1;
  PrintCentre("Puissance W : " + Ascii(nomSondeMobile), WE / 2, 5, 1.4);
  lcd->setTextColor(CoulGrTexte, CoulGrFond);
  lcd->fillRect(0, 35, WE, HE - 35, CoulGrFond);
  int Maxi = 0;
  if (gr == 10) {  //10mn
    for (int i = 0; i < 300; i++) {
      Maxi = max(Maxi, abs(tabPw_Maison_2s[i]));
    }
  } else {  //48h
    for (int i = 0; i < 600; i++) {
      Maxi = max(Maxi, abs(tabPw_Maison_5mn[i]));
    }
  }

  int step = 500;
  if (Maxi > 2000) step = 1000;
  Maxi = step * int(Maxi / step) + step;

  if (gr == 10) {  //10mn

    iS1 = IdxStock2s;
    delta = (HE - 70) / 2;
    for (int i = 0; i < 299; i++) {
      Y1 = Hm - delta * tabPw_Maison_2s[iS1] / Maxi;
      X1 = 40 + (WE - 50) * i / 300;
      if (Y1 <= Hm) {
        lcd->drawLine(X1, Y1, X1, Hm, CoulW);
      } else {
        lcd->drawLine(X1, Y1, X1, Hm, Cinject);
      }
      iS1 = (iS1 + 1) % 300;
    }

    delta = (WE - 50) / 10;
    for (int i = 1; i <= 10; i++) {
      lcd->drawFastVLine(40 + i * delta, Hm, 5, CoulGrTexte);
      if (i % 2 == 0) PrintCentre(String(i - 10), 40 + i * delta, Hm + 5, 0.8);
    }
    PrintCentre("mn", WE - 10, Hm - 20, 0.8);

  } else {
    iS1 = IdxStockPW;
    delta = (HE - 70) / 2;
    for (int i = 0; i < 599; i++) {
      Y1 = Hm - delta * tabPw_Maison_5mn[iS1] / Maxi;
      X1 = 40 + (WE - 50) * i / 600;
      if (Y1 <= Hm) {
        lcd->drawLine(X1, Y1, X1, Hm, CoulW);
      } else {
        lcd->drawLine(X1, Y1, X1, Hm, Cinject);
      }
      iS1 = (iS1 + 1) % 600;
    }
    delta = (WE - 50) / 48;
    int mn = (HeureCouranteDeci % 100) * delta;
    mn = WE - 10 - int(mn / 100);
    int h = int(HeureCouranteDeci / 100);
    for (int i = 0; i < 48; i++) {
      if (h % 6 == 0) {
        lcd->drawFastVLine(mn, Hm, 5, CoulGrTexte);
        PrintCentre(String(h), mn, Hm + 5, 0.8);
      }
      mn += -delta;
      h = (h + 23) % 24;
    }
    PrintCentre("h", WE - 10, Hm - 20, 0.8);
  }



  lcd->drawFastHLine(40, Hm, WE - 50, CoulGrTexte);
  lcd->drawFastVLine(40, 45, HE - 70, CoulGrTexte);
  int tick = Maxi / step;
  delta = (HE - 70) / (2 * tick);
  for (int i = -tick; i <= tick; i++) {
    lcd->drawFastHLine(35, Hm - delta * i, 5, CoulGrTexte);
    PrintDroite(String(i * step), 35, Hm - delta * i - 10, 0.8);
  }

  TraceDate();
  TraceTarif();
}




void TraceMessages() {
  int j = idxMessage;
  int k = 0;
  lcd->setTextColor(CoulTexte, CoulFond);
  PrintCentre("Messages", -1, 0, 1.2);
  lcd->setCursor(0, 30);
  lcd->setTextSize(1);
  j = idxMessage + idxDecalM;
  for (int i = idxDecalM; i < 10; i++) {
    lcd->fillRect(0, lcd->getCursorY(), lcd->width(), 80, CoulFond);
    j = j % 10;
    if (not(MessageH[j].isEmpty())) {
      lcd->println(Ascii(MessageH[j]));
    }
    j++;
  }
  if (lcd->getCursorY() > lcd->height() - 10) idxDecalM++;
  if (idxDecalM > 0 && lcd->getCursorY() < lcd->height() - 40) idxDecalM--;
}
void TraceReseau() {
  lcd->setTextColor(CoulTexte, CoulFond);
  lcd->fillRect(0, 0, lcd->width(), lcd->height(), CoulFond);
  PrintCentre(Ascii("Réseau"), -1, 10, 2);
  lcd->println("");
  lcd->setTextSize(1);
  lcd->println("");
  if (WiFi.getMode() == WIFI_STA) {
    lcd->println(" Niveau WiFi : " + String(WiFi.RSSI()) + " dBm");
    lcd->println(Ascii(" Point d'accès WiFi : ") + WiFi.BSSIDstr());
    lcd->println(" Adresse MAC : " + WiFi.macAddress());
    lcd->println(Ascii(" Réseau WiFi : ") + ssid);
    lcd->println(" Adresse IP ESP32 : " + WiFi.localIP().toString());
    lcd->println(" Adresse passerelle : " + WiFi.gatewayIP().toString());
    lcd->println(Ascii(" Masque du réseau : ") + WiFi.subnetMask().toString());
  } else {
    lcd->println(" Connectez vous au WiFi : ");
    lcd->println(Ascii(" Mode Point d'Accès : ") + hostname);
    lcd->print(" Adresse IP ESP32 : ");
    lcd->println(WiFi.softAPIP());
  }
}

void TraceCalibr() {
  if (ESP32_Type <8) {
    lcd->fillScreen(CoulFond);
    lcd->setTextColor(CoulTexte, CoulFond);
    PrintCentre("Calibration", -1, lcd->height() / 2, 2.5);
    PrintCentre("Cliquez dans le coin", -1, lcd->height() / 2 + 50, 2);
    delay(1000);
    lcd->calibrateTouch(Calibre, TFT_MAGENTA, TFT_WHITE, 30);  // Runs a test that has you touch the corners of the screen
    lcd->setTouchCalibrate(Calibre);                           // setTouch actually implements the data form calibrateTouch
    lcd->fillScreen(CoulFond);
    EcritureEnROM();
    ClickCount = 0;
    GoPage(0);
  }
}
void TraceWcolor() {
  unsigned long CF = WattToColor();
  lcd->fillScreen(CF);
  lcd->setTextColor(CoulTexte, CF);
  PrintCentre(String(PuissanceS_M - PuissanceI_M) + " W", lcd->width() / 2, lcd->height() / 2.7, 4);
  for (int i = 0; i < NbActions; i++) {
    if (LesActions[i].Actif != MODE_INACTIF) {
      String S = LesActions[i].Titre + " : " + String(100 - Retard[i]) + "%";
      PrintDroite(S, lcd->width() - 2, 2, 1.5);
      i=NbActions;
    }
  }
  TraceDate();
  TraceTarif();
}
void TraceGaugeW() {
  int C=lcd->height() / 1.5;
  int W=lcd->width();
  int W2=W/2;
  float Teta0,Teta1;
  int R0=min(lcd->width(),lcd->height());
  R0=R0/3.1;
  int R1=1.5*R0;
  lcd->fillScreen(CoulFond);
  lcd->setTextColor(CoulTexte, CoulFond);
  Teta0=-180;
  Teta1=Teta0+180*2500/9000;
  lcd->fillArc(W2,C,R0,R1,Teta0,Teta1,TFT_BLUE);
  Teta0=Teta1;
  Teta1=Teta0+180/9;
  lcd->fillArc(W2,C,R0,R1,Teta0,Teta1,TFT_GREEN);
  Teta0=Teta1;
  Teta1=Teta0+180*35/90;
  lcd->fillArc(W2,C,R0,R1,Teta0,Teta1,TFT_ORANGE);
  Teta0=Teta1;
  Teta1=0.0;
  lcd->fillArc(W2,C,R0,R1,Teta0,Teta1,TFT_RED);
  PrintCentre(String(PuissanceS_M - PuissanceI_M) + " W", W2, C, 4);
  for (int i = 0; i < NbActions; i++) {
    if (LesActions[i].Actif != MODE_INACTIF) {
      String S = LesActions[i].Titre ;
      PrintDroite(S, lcd->width() - 50, 2, 1.5);
      lcd->fillCircle(lcd->width() - 22,22,18,CoulTexte);
      Teta0=-90;
      Teta1=Teta0 + 3.60*(100-Retard[i]);
      lcd->fillArc(lcd->width() - 22,22,0,18,Teta0,Teta1,CoulW);
      i=NbActions;
    }
  }
  Teta0=-PI + PI*float(PuissanceS_M - PuissanceI_M+3000.0)/9000.0;
  Teta0=min(Teta0,float(0));
  Teta0=max(float(-PI),Teta0);
  R0=0.5*R0;
  lcd->fillTriangle(W2+R1*cos(Teta0),C+R1*sin(Teta0),W2+R0*cos(Teta0-0.2),C+R0*sin(Teta0-0.2),W2+R0*cos(Teta0+0.2),C+R0*sin(Teta0+0.2),CoulTexte); //Aiguilles
  TraceDate();
  TraceTarif();
}
void TraceTarif() {
  String Tarif[5] = { "PLEINE", "CREUSE", "BLEU", "BLANC", "ROUGE" };
  uint16_t couleur[5] = { TFT_RED, TFT_GREEN, TFT_BLUE, TFT_WHITE, TFT_RED };
  int16_t i;
  int16_t W, H;
  W = 20 + 20 * float(rotation % 2);
  H = 30 - 10 * float(rotation % 2);
  if (LTARF != "") {
    lcd->drawRect(lcd->width() - W * 3, lcd->height() - H, 3 * W - 1, H - 1, CoulSaisieBord);
    for (i = 0; i < 5; i++) {
      if (LTARF.indexOf(Tarif[i]) >= 0) {
        lcd->fillRect(lcd->width() - W * 3 + 1, lcd->height() - H + 1, 3 * W - 2, H - 2, couleur[i]);
      }
    }
  }

  long lendemain = int(strtol(STGEt.c_str(), NULL, 16)) / 4;
  if (lendemain > 0) {
    lcd->drawRect(lcd->width() - W, lcd->height() - H, W - 1, H - 1, CoulSaisieBord);
    lcd->fillRect(lcd->width() - W + 1, lcd->height() - H + 1, W - 2, H - 2, couleur[lendemain + 1]);
  }
}
void TraceDate() {
  lcd->setCursor(2, lcd->height() - 20);
  lcd->setTextSize(1);
  lcd->print(DATE);
}
void PrintCentre(String S, int X, int Y, float Sz) {
  if (X < 0) X = lcd->width() / 2;
  lcd->setTextSize(Sz);
  int W = lcd->textWidth(S);
  lcd->setCursor(X - W / 2, Y + 3);
  lcd->print(S);
}
void PrintGauche(String S, int X, int Y, float Sz) {
  lcd->setTextSize(Sz);
  lcd->setCursor(X + 4, Y + 3);
  lcd->print(S);
}
void PrintDroite(String S, int X, int Y, float Sz) {
  lcd->setTextSize(Sz);
  int W = lcd->textWidth(S);
  lcd->setCursor(X - W - 4, Y + 3);
  lcd->print(S);
}

String Ascii(String S) {
  S.replace("é", String(char(130)));
  S.replace("â", String(char(131)));
  S.replace("à", String(char(133)));
  S.replace("ç", String(char(135)));
  S.replace("ê", String(char(136)));
  S.replace("è", String(char(138)));
  S.replace("ù", String(char(151)));
  return S;
}
