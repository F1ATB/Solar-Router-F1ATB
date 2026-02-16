

#include <Arduino.h>
String BIDON;
String BIDON3 = "essai3";
String Record_Conf = "";
void INIT_EEPROM(void) {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    StockMessage("Failed to initialise EEPROM");
    delay(10000);
    ReseT("Failed to initialise EEPROM");
  }
}

void RAZ_Histo_Conso() {
  //Mise a zero Zone stockage
  int Adr_SoutInjec = adr_HistoAn;
  for (int i = 0; i < NbJour; i++) {
    EEPROM.writeLong(Adr_SoutInjec, 0);
    Adr_SoutInjec = Adr_SoutInjec + 4;
  }
  EEPROM.writeULong(adr_E_T_soutire0, 0);
  EEPROM.writeULong(adr_E_T_injecte0, 0);
  EEPROM.writeULong(adr_E_M_soutire0, 0);
  EEPROM.writeULong(adr_E_M_injecte0, 0);
  EEPROM.writeString(adr_DateCeJour, "");
  EEPROM.writeUShort(adr_lastStockConso, 0);
  EEPROM.commit();
}

void LectureConsoMatinJour(void) {

  Energie_jour_Soutiree = 0;  // en Wh
  Energie_jour_Injectee = 0;  // en Wh

  EAS_T_J0 = EEPROM.readULong(adr_E_T_soutire0);  //Triac
  EAI_T_J0 = EEPROM.readULong(adr_E_T_injecte0);
  EAS_M_J0 = EEPROM.readULong(adr_E_M_soutire0);  //Maison
  EAI_M_J0 = EEPROM.readULong(adr_E_M_injecte0);
  //DateCeJour = EEPROM.readString(adr_DateCeJour); Plus utilisé depuis V13
  idxPromDuJour = EEPROM.readUShort(adr_lastStockConso);
  if (Energie_T_Soutiree < EAS_T_J0) {
    Energie_T_Soutiree = EAS_T_J0;
  }
  if (Energie_T_Injectee < EAI_T_J0) {
    Energie_T_Injectee = EAI_T_J0;
  }
  if (Energie_M_Soutiree < EAS_M_J0) {
    Energie_M_Soutiree = EAS_M_J0;
  }
  if (Energie_M_Injectee < EAI_M_J0) {
    Energie_M_Injectee = EAI_M_J0;
  }
}



String HistoriqueEnergie1An(void) {
  JsonDocument conf;
  int i;
  //Energie Soutire-Injecté sur 370 jours
  int Adr_SoutInjec = 0;
  long EnergieJour = 0;
  long DeltaEnergieJour = 0;
  int iS = 0;
  long lastDay = 0;
  String ligne = "";

  for (i = 0; i < NbJour; i++) {
    iS = (idxPromDuJour + i + 1) % NbJour;
    Adr_SoutInjec = adr_HistoAn + iS * 4;
    EnergieJour = EEPROM.readLong(Adr_SoutInjec);
    if (lastDay == 0) { lastDay = EnergieJour; }
    DeltaEnergieJour = EnergieJour - lastDay;
    lastDay = EnergieJour;
    conf["Energie1an"][i] = DeltaEnergieJour;
  }

  //Vue par jour/mois Soutiré et Injecté
  int M0 = DateAMJ.substring(4, 6).toInt();
  int an0 = DateAMJ.substring(0, 4).toInt();
  i = 0;
  for (int M = -2; M <= 0; M++) {  //3 derniers mois
    int M1 = M0 + M;
    int an1 = an0;
    if (M1 < 1) {
      M1 = M1 + 12;
      an1 = an0 - 1;
    }
    M1 = M1 + 100;
    String SM1 = String(M1);
    String AM_file = "/Mois_Wh_" + String(an1) + SM1.substring(1, 3) + ".csv";
    if (LittleFS.exists(AM_file)) {

      File file = LittleFS.open(AM_file);
      while (file.available()) {
        char c = file.read();
        if (c == '\n' || c == '\r') {
          if (ligne.length() > 10) {
            if (ligne.indexOf("Date,") < 0) {
              ligne.trim();
              conf["EnergieJour"][i] = ligne;
              i++;
            }
            ligne = "";
          }

        } else {
          ligne += String(c);
        }
      }
      file.close();
    }
  }
  if (i == 0) {  //Rien trouvé
    conf["EnergieJour"] = "";
  }
  String Json;
  serializeJson(conf, Json);
  return Json;
}
unsigned long LectureCle() {
  return EEPROM.readULong(adr_ParaActions);
}
void LectureEnROM() {
  int Hdeb;
  int address = adr_ParaActions;
  int VersionStocke;
  Cle_ROM = EEPROM.readULong(address);
  address += sizeof(unsigned long);
  VersionStocke = EEPROM.readUShort(address);
  address += sizeof(unsigned short);
  ssid = EEPROM.readString(address);
  address += ssid.length() + 1;
  password = EEPROM.readString(address);
  address += password.length() + 1;
  dhcpOn = EEPROM.readByte(address);
  address += sizeof(byte);
  RMS_IP[0] = EEPROM.readULong(address);
  address += sizeof(unsigned long);
  Gateway = EEPROM.readULong(address);
  address += sizeof(unsigned long);
  masque = EEPROM.readULong(address);
  address += sizeof(unsigned long);
  dns = EEPROM.readULong(address);
  address += sizeof(unsigned long);
  CleAccesRef = EEPROM.readString(address);
  address += CleAccesRef.length() + 1;
  Couleurs = EEPROM.readString(address);
  address += Couleurs.length() + 1;
  ModePara = EEPROM.readByte(address);
  address += sizeof(byte);
  ModeReseau = EEPROM.readByte(address);
  address += sizeof(byte);
  Horloge = EEPROM.readByte(address);
  address += sizeof(byte);
  ESP32_Type = EEPROM.readByte(address);
  address += sizeof(byte);
  int VersionMajeur = int(VersionStocke / 100);
  String V = Version;
  int Vr = V.toInt();
  TelnetPrint("Version stockée (partie entière) :");
  TelnetPrintln(String(VersionMajeur));
  TelnetPrint("Version du logiciel( partie entière) :");
  TelnetPrintln(String(Vr));
  if (Vr == VersionMajeur) {  //La partie entière  ne change pas. On lit la suite
    LEDgroupe = EEPROM.readByte(address);
    address += sizeof(byte);
    rotation = EEPROM.readByte(address);
    address += sizeof(byte);
    DurEcran = EEPROM.readULong(address);
    address += sizeof(unsigned long);
    for (int i = 0; i < 8; i++) {
      Calibre[i] = EEPROM.readShort(address);
      address += sizeof(unsigned short);
    }
    pUxI = EEPROM.readByte(address);
    address += sizeof(byte);
    pTemp = EEPROM.readByte(address);
    address += sizeof(byte);
    Source = EEPROM.readString(address);
    address += Source.length() + 1;
    RMSextIP = EEPROM.readULong(address);
    address += sizeof(unsigned long);
    EnphaseUser = EEPROM.readString(address);
    address += EnphaseUser.length() + 1;
    EnphasePwd = EEPROM.readString(address);
    address += EnphasePwd.length() + 1;
    EnphaseSerial = EEPROM.readString(address);
    address += EnphaseSerial.length() + 1;
    MQTTRepet = EEPROM.readUShort(address);
    address += sizeof(unsigned short);
    MQTTIP = EEPROM.readULong(address);
    address += sizeof(unsigned long);
    MQTTPort = EEPROM.readUShort(address);
    address += sizeof(unsigned short);
    MQTTUser = EEPROM.readString(address);
    address += MQTTUser.length() + 1;
    MQTTPwd = EEPROM.readString(address);
    address += MQTTPwd.length() + 1;
    MQTTPrefix = EEPROM.readString(address);
    address += MQTTPrefix.length() + 1;
    MQTTPrefixEtat = EEPROM.readString(address);
    address += MQTTPrefixEtat.length() + 1;
    MQTTdeviceName = EEPROM.readString(address);
    address += MQTTdeviceName.length() + 1;
    TopicP = EEPROM.readString(address);
    address += TopicP.length() + 1;
    subMQTT = EEPROM.readByte(address);
    address += sizeof(byte);
    nomRouteur = EEPROM.readString(address);
    address += nomRouteur.length() + 1;
    nomSondeFixe = EEPROM.readString(address);
    address += nomSondeFixe.length() + 1;
    nomSondeMobile = EEPROM.readString(address);
    address += nomSondeMobile.length() + 1;
    for (int i = 1; i < LES_ROUTEURS_MAX; i++) {
      RMS_IP[i] = EEPROM.readULong(address);
      address += sizeof(unsigned long);
    }
    for (int c = 0; c < 4; c++) {
      nomTemperature[c] = EEPROM.readString(address);
      address += nomTemperature[c].length() + 1;
      Source_Temp[c] = EEPROM.readString(address);
      address += Source_Temp[c].length() + 1;
      refTempIP[c] = EEPROM.readByte(address);
      address += sizeof(byte);
      TopicT[c] = EEPROM.readString(address);
      address += TopicT[c].length() + 1;
      canalTempExterne[c] = EEPROM.readByte(address);
      address += sizeof(byte);
      offsetTemp[c] = EEPROM.readShort(address);
      address += sizeof(unsigned short);
    }
    CalibU = EEPROM.readUShort(address);
    address += sizeof(unsigned short);
    CalibI = EEPROM.readUShort(address);
    address += sizeof(unsigned short);
    TempoRTEon = EEPROM.readByte(address);
    address += sizeof(byte);
    WifiSleep = EEPROM.readByte(address);
    address += sizeof(byte);
    ComSurv = EEPROM.readShort(address);
    address += sizeof(short);
    pSerial = EEPROM.readByte(address);
    address += sizeof(byte);
    pTriac = EEPROM.readByte(address);
    address += sizeof(byte);

    //Zone des actions
    ReacCACSI = EEPROM.readByte(address);
    address += sizeof(byte);
    Fpwm = EEPROM.readUShort(address);
    address += sizeof(unsigned short);
    NbActions = EEPROM.readUShort(address);
    address += sizeof(unsigned short);
    for (int iAct = 0; iAct < NbActions; iAct++) {
      LesActions[iAct].Actif = EEPROM.readByte(address);
      address += sizeof(byte);
      LesActions[iAct].Titre = EEPROM.readString(address);
      address += LesActions[iAct].Titre.length() + 1;
      LesActions[iAct].Host = EEPROM.readString(address);
      address += LesActions[iAct].Host.length() + 1;
      LesActions[iAct].Port = EEPROM.readUShort(address);
      address += sizeof(unsigned short);
      LesActions[iAct].OrdreOn = EEPROM.readString(address);
      address += LesActions[iAct].OrdreOn.length() + 1;
      LesActions[iAct].OrdreOn.replace(String((char)31), "|");  //Depuis la V10.01 (Input separator)
      LesActions[iAct].OrdreOff = EEPROM.readString(address);
      address += LesActions[iAct].OrdreOff.length() + 1;
      LesActions[iAct].Repet = EEPROM.readUShort(address);
      address += sizeof(unsigned short);
      LesActions[iAct].Tempo = EEPROM.readUShort(address);
      address += sizeof(unsigned short);
      LesActions[iAct].Kp = EEPROM.readByte(address);
      address += sizeof(byte);
      LesActions[iAct].Ki = EEPROM.readByte(address);
      address += sizeof(byte);
      LesActions[iAct].Kd = EEPROM.readByte(address);
      address += sizeof(byte);
      LesActions[iAct].PID = EEPROM.readByte(address);
      address += sizeof(byte);
      LesActions[iAct].NbPeriode = EEPROM.readByte(address);
      address += sizeof(byte);
      Hdeb = 0;
      for (byte i = 0; i < LesActions[iAct].NbPeriode; i++) {
        LesActions[iAct].Type[i] = EEPROM.readByte(address);
        address += sizeof(byte);
        LesActions[iAct].Hfin[i] = EEPROM.readUShort(address);
        LesActions[iAct].Hdeb[i] = Hdeb;
        Hdeb = LesActions[iAct].Hfin[i];
        address += sizeof(short);
        LesActions[iAct].Vmin[i] = EEPROM.readShort(address);
        address += sizeof(short);
        LesActions[iAct].Vmax[i] = EEPROM.readShort(address);
        address += sizeof(short);
        LesActions[iAct].Tinf[i] = EEPROM.readShort(address);
        address += sizeof(short);
        LesActions[iAct].Tsup[i] = EEPROM.readShort(address);
        address += sizeof(short);
        LesActions[iAct].Hmin[i] = EEPROM.readShort(address);
        address += sizeof(short);
        LesActions[iAct].Hmax[i] = EEPROM.readShort(address);
        address += sizeof(short);
        LesActions[iAct].CanalTemp[i] = EEPROM.readShort(address);
        address += sizeof(short);
        LesActions[iAct].SelAct[i] = EEPROM.readByte(address);
        address += sizeof(byte);
        LesActions[iAct].Ooff[i] = EEPROM.readByte(address);
        address += sizeof(byte);
        LesActions[iAct].O_on[i] = EEPROM.readByte(address);
        address += sizeof(byte);
        LesActions[iAct].Tarif[i] = EEPROM.readByte(address);
        address += sizeof(byte);
      }
    }
    Calibration(address);
  }
}
int EcritureEnROM() {
  int address = adr_ParaActions;
  int VersionStocke = 0;
  String V = Version;
  VersionStocke = round(100 * V.toFloat());
  EEPROM.writeULong(address, Cle_ROM);
  address += sizeof(unsigned long);
  EEPROM.writeUShort(address, VersionStocke);
  address += sizeof(unsigned short);
  EEPROM.writeString(address, ssid);
  address += ssid.length() + 1;
  EEPROM.writeString(address, password);
  address += password.length() + 1;
  EEPROM.writeByte(address, dhcpOn);
  address += sizeof(byte);
  EEPROM.writeULong(address, RMS_IP[0]);
  address += sizeof(unsigned long);
  EEPROM.writeULong(address, Gateway);
  address += sizeof(unsigned long);
  EEPROM.writeULong(address, masque);
  address += sizeof(unsigned long);
  EEPROM.writeULong(address, dns);
  address += sizeof(unsigned long);
  EEPROM.writeString(address, CleAccesRef);
  address += CleAccesRef.length() + 1;
  EEPROM.writeString(address, Couleurs);
  address += Couleurs.length() + 1;
  EEPROM.writeByte(address, ModePara);
  address += sizeof(byte);
  EEPROM.writeByte(address, ModeReseau);
  address += sizeof(byte);
  EEPROM.writeByte(address, Horloge);
  address += sizeof(byte);
  EEPROM.writeByte(address, ESP32_Type);
  address += sizeof(byte);
  EEPROM.writeByte(address, LEDgroupe);
  address += sizeof(byte);
  EEPROM.writeByte(address, rotation);
  address += sizeof(byte);
  EEPROM.writeULong(address, DurEcran);
  address += sizeof(unsigned long);
  for (int i = 0; i < 8; i++) {
    EEPROM.writeShort(address, Calibre[i]);
    address += sizeof(unsigned short);
  }
  EEPROM.writeByte(address, pUxI);
  address += sizeof(byte);
  EEPROM.writeByte(address, pTemp);
  address += sizeof(byte);
  EEPROM.writeString(address, Source);
  address += Source.length() + 1;
  EEPROM.writeULong(address, RMSextIP);
  address += sizeof(unsigned long);
  EEPROM.writeString(address, EnphaseUser);
  address += EnphaseUser.length() + 1;
  EEPROM.writeString(address, EnphasePwd);
  address += EnphasePwd.length() + 1;
  EEPROM.writeString(address, EnphaseSerial);
  address += EnphaseSerial.length() + 1;
  if (ModePara == 0) {
    MQTTRepet = 0;
    subMQTT = 0;
  }
  EEPROM.writeUShort(address, MQTTRepet);
  address += sizeof(unsigned short);
  EEPROM.writeULong(address, MQTTIP);
  address += sizeof(unsigned long);
  EEPROM.writeUShort(address, MQTTPort);
  address += sizeof(unsigned short);
  EEPROM.writeString(address, MQTTUser);
  address += MQTTUser.length() + 1;
  EEPROM.writeString(address, MQTTPwd);
  address += MQTTPwd.length() + 1;
  EEPROM.writeString(address, MQTTPrefix);
  address += MQTTPrefix.length() + 1;
  EEPROM.writeString(address, MQTTPrefixEtat);
  address += MQTTPrefixEtat.length() + 1;
  EEPROM.writeString(address, MQTTdeviceName);
  address += MQTTdeviceName.length() + 1;
  EEPROM.writeString(address, TopicP);
  address += TopicP.length() + 1;
  EEPROM.writeByte(address, subMQTT);
  address += sizeof(byte);
  EEPROM.writeString(address, nomRouteur);
  address += nomRouteur.length() + 1;
  EEPROM.writeString(address, nomSondeFixe);
  address += nomSondeFixe.length() + 1;
  EEPROM.writeString(address, nomSondeMobile);
  address += nomSondeMobile.length() + 1;
  for (int i = 1; i < LES_ROUTEURS_MAX; i++) {
    EEPROM.writeULong(address, RMS_IP[i]);
    address += sizeof(unsigned long);
  }
  for (int c = 0; c < 4; c++) {
    EEPROM.writeString(address, nomTemperature[c]);
    address += nomTemperature[c].length() + 1;
    EEPROM.writeString(address, Source_Temp[c]);
    address += Source_Temp[c].length() + 1;
    EEPROM.writeByte(address, refTempIP[c]);
    address += sizeof(byte);
    EEPROM.writeString(address, TopicT[c]);
    address += TopicT[c].length() + 1;
    EEPROM.writeByte(address, canalTempExterne[c]);
    address += sizeof(byte);
    EEPROM.writeShort(address, offsetTemp[c]);
    address += sizeof(unsigned short);
  }
  EEPROM.writeUShort(address, CalibU);
  address += sizeof(unsigned short);
  EEPROM.writeUShort(address, CalibI);
  address += sizeof(unsigned short);
  EEPROM.writeByte(address, TempoRTEon);
  address += sizeof(byte);
  EEPROM.writeByte(address, WifiSleep);
  address += sizeof(byte);
  EEPROM.writeShort(address, ComSurv);
  address += sizeof(unsigned short);
  EEPROM.writeByte(address, pSerial);
  address += sizeof(byte);
  EEPROM.writeByte(address, pTriac);
  address += sizeof(byte);
  //Enregistrement des Actions
  EEPROM.writeByte(address, ReacCACSI);
  address += sizeof(byte);
  EEPROM.writeUShort(address, Fpwm);
  address += sizeof(short);
  EEPROM.writeUShort(address, NbActions);
  address += sizeof(unsigned short);
  for (int iAct = 0; iAct < NbActions; iAct++) {
    EEPROM.writeByte(address, LesActions[iAct].Actif);
    address += sizeof(byte);
    EEPROM.writeString(address, LesActions[iAct].Titre);
    address += LesActions[iAct].Titre.length() + 1;
    EEPROM.writeString(address, LesActions[iAct].Host);
    address += LesActions[iAct].Host.length() + 1;
    EEPROM.writeUShort(address, LesActions[iAct].Port);
    address += sizeof(unsigned short);
    EEPROM.writeString(address, LesActions[iAct].OrdreOn);
    address += LesActions[iAct].OrdreOn.length() + 1;
    EEPROM.writeString(address, LesActions[iAct].OrdreOff);
    address += LesActions[iAct].OrdreOff.length() + 1;
    EEPROM.writeUShort(address, LesActions[iAct].Repet);
    address += sizeof(unsigned short);
    EEPROM.writeUShort(address, LesActions[iAct].Tempo);
    address += sizeof(unsigned short);
    EEPROM.writeByte(address, LesActions[iAct].Kp);
    address += sizeof(byte);
    EEPROM.writeByte(address, LesActions[iAct].Ki);
    address += sizeof(byte);
    EEPROM.writeByte(address, LesActions[iAct].Kd);
    address += sizeof(byte);
    EEPROM.writeByte(address, LesActions[iAct].PID);
    address += sizeof(byte);
    EEPROM.writeByte(address, LesActions[iAct].NbPeriode);
    address += sizeof(byte);
    for (byte i = 0; i < LesActions[iAct].NbPeriode; i++) {
      if (ModePara == 0) {  //standard
        LesActions[iAct].CanalTemp[i] = -1;
        LesActions[iAct].SelAct[i] = 255;
      }
      EEPROM.writeByte(address, LesActions[iAct].Type[i]);
      address += sizeof(byte);
      EEPROM.writeUShort(address, LesActions[iAct].Hfin[i]);
      address += sizeof(short);
      EEPROM.writeShort(address, LesActions[iAct].Vmin[i]);
      address += sizeof(short);
      EEPROM.writeShort(address, LesActions[iAct].Vmax[i]);
      address += sizeof(short);
      EEPROM.writeShort(address, LesActions[iAct].Tinf[i]);
      address += sizeof(short);
      EEPROM.writeShort(address, LesActions[iAct].Tsup[i]);
      address += sizeof(short);
      EEPROM.writeShort(address, LesActions[iAct].Hmin[i]);
      address += sizeof(short);
      EEPROM.writeShort(address, LesActions[iAct].Hmax[i]);
      address += sizeof(short);
      EEPROM.writeShort(address, LesActions[iAct].CanalTemp[i]);
      address += sizeof(short);
      EEPROM.writeByte(address, LesActions[iAct].SelAct[i]);
      address += sizeof(byte);
      EEPROM.writeByte(address, LesActions[iAct].Ooff[i]);
      address += sizeof(byte);
      EEPROM.writeByte(address, LesActions[iAct].O_on[i]);
      address += sizeof(byte);
      EEPROM.writeByte(address, LesActions[iAct].Tarif[i]);
      address += sizeof(byte);
    }
  }
  Calibration(address);
  EEPROM.commit();
  RecordFichierParametres();
  return address;
}
void Calibration(int address) {
  kV = KV * float(CalibU) / 1000.0;  //Calibration coefficient to be applied
  kI = KI * float(CalibI) / 1000.0;
  P_cent_EEPROM = int(100 * address / EEPROM_SIZE);
  TelnetPrintln("Mémoire EEPROM utilisée : " + String(P_cent_EEPROM) + "%");
}

void init_puissance() {
  PuissanceS_T = 0;
  PuissanceS_M = 0;
  PuissanceI_T = 0;
  PuissanceI_M = 0;  //Puissance Watt affichée en entiers Maison et Triac
  PVAS_T = 0;
  PVAS_M = 0;
  PVAI_T = 0;
  PVAI_M = 0;  //Puissance VA affichée en entiers Maison et Triac
  PuissanceS_T_inst = 0.0;
  PuissanceS_M_inst = 0.0;
  PuissanceI_T_inst = 0.0;
  PuissanceI_M_inst = 0.0;
  PVAS_T_inst = 0.0;
  PVAS_M_inst = 0.0;
  PVAI_T_inst = 0.0;
  PVAI_M_inst = 0.0;
  Puissance_T_moy = 0.0;
  Puissance_M_moy = 0.0;
  PVA_T_moy = 0.0;
  PVA_M_moy = 0.0;
}
void filtre_puissance() {  //Filtre RC

  float A = 0.3;  //Coef pour un lissage en multi-sinus et train de sinus sur les mesures de puissance courte
  float B = 0.7;
  if (!LissageLong) {
    A = 1;
    B = 0;
  }

  Puissance_T_moy = A * (PuissanceS_T_inst - PuissanceI_T_inst) + B * Puissance_T_moy;
  if (Puissance_T_moy < 0) {
    PuissanceI_T = -int(Puissance_T_moy);  //Puissance Watt affichée en entier  Triac
    PuissanceS_T = 0;
  } else {
    PuissanceS_T = int(Puissance_T_moy);
    PuissanceI_T = 0;
  }



  Puissance_M_moy = A * (PuissanceS_M_inst - PuissanceI_M_inst) + B * Puissance_M_moy;
  if (Puissance_M_moy < 0) {
    PuissanceI_M = -int(Puissance_M_moy);  //Puissance Watt affichée en entier Maison
    PuissanceS_M = 0;
  } else {
    PuissanceS_M = int(Puissance_M_moy);
    PuissanceI_M = 0;
  }


  PVA_T_moy = A * (PVAS_T_inst - PVAI_T_inst) + B * PVA_T_moy;  //Puissance VA affichée en entiers
  if (PVA_T_moy < 0) {
    PVAI_T = -int(PVA_T_moy);
    PVAS_T = 0;
  } else {
    PVAS_T = int(PVA_T_moy);
    PVAI_T = 0;
  }

  PVA_M_moy = A * (PVAS_M_inst - PVAI_M_inst) + B * PVA_M_moy;
  if (PVA_M_moy < 0) {
    PVAI_M = -int(PVA_M_moy);
    PVAS_M = 0;
  } else {
    PVAS_M = int(PVA_M_moy);
    PVAI_M = 0;
  }
}

void StockMessage(String m) {
  if (DATE != "") m = DATE + " : " + m;
  MessageH[idxMessage] = m;
  idxMessage = (idxMessage + 1) % 10;
  PrintScroll(m);
}

// *************************************************************
// Stockage parametres en zone SPIFFS (methode 2026 depuis V17)
// *************************************************************
void RecordFichierParametres() {
  File file = LittleFS.open("/parametres.json", FILE_WRITE);
  file.print(SerializeConfiguration());  //Fichier au format JSON
  file.close();
  Serial.println("Ecriture fichier parametres");
}
void ReadFichierParametres() {
  if (!LittleFS.exists("/parametres.json")) {  //Fichier pas encore crée
    RecordFichierParametres();
  }

  File file = LittleFS.open("/parametres.json", "r");
  Serial.println("Lecture du fichier paramètres");
  String content = file.readString();  // lit tout le fichier
  file.close();
  DeserializeConfiguration(content);
}
void StockFichier(String filename, String Contenu) {  //Fichier de données
  File file = LittleFS.open("/" + filename, FILE_WRITE);
  file.print(Contenu);
  file.close();
  Serial.println("Ecriture fichier : " + filename);
}
//Importation des paramètres
//***************************
void ImportParametres(String Conf) {
  DeserializeConfiguration(Conf);
  EcritureEnROM();
}
void DeserializeConfiguration(String json) {

  int16_t Hdeb;
  Serial.print("Json reçu:");
  Serial.println(json);
  JsonDocument conf;
  DeserializationError error = deserializeJson(conf, json);

  if (error) {
    Serial.print("Erreur de parsing des paramètres: ");
    Serial.println(error.c_str());
    return;
  }
  
  String V = Version;
 
  int Versioncompile = round(100 * V.toFloat());
  int VersionStocke = conf["VersionStocke"];
  ssid = conf["ssid"].as<String>();
  password = conf["password"].as<String>();
  dhcpOn = conf["dhcpOn"];
  Gateway = conf["Gateway"];
  masque = conf["masque"];
  dns = conf["dns"];
  hostname = conf["hostname"] | hostname;
  RMS_IP[0] = conf["IP_Fixe"];
  CleAccesRef = conf["CleAccesRef"] | CleAccesRef;
  hostname = conf["hostname"] | hostname;
  Couleurs = conf["Couleurs"] | Couleurs;
  if (Couleurs.length() == 0) Couleurs = String(CouleurDefaut);
  ModePara = conf["ModePara"];
  ModeReseau = conf["ModeReseau"];
  Horloge = conf["Horloge"];
  idxFuseau = conf["idxFuseau"].isNull() ? idxFuseau : conf["idxFuseau"];
  ntpServer = conf["ntpServer"] | ntpServer;
  ESP32_Type = conf["ESP32_Type"];
  LEDgroupe = conf["LEDgroupe"];
  rotation = conf["rotation"];
  DurEcran = conf["DurEcran"];
  clickPresence = conf["clickPresence"].isNull() ? clickPresence : conf["clickPresence"];
  NumPageBoot = conf["NumPageBoot"].isNull() ? NumPageBoot : conf["NumPageBoot"];
  for (int i = 0; i < 8; i++) {
    Calibre[i] = conf["Calibre" + String(i)];
  }
  pUxI = conf["pUxI"];
  pTemp = conf["pTemp"];
  Source = conf["Source"].as<String>();
  RMSextIP = conf["RMSextIP"];
  EnphaseUser = conf["EnphaseUser"].as<String>();
  EnphasePwd = conf["EnphasePwd"].as<String>();
  EnphaseSerial = conf["EnphaseSerial"].as<String>();
  MQTTRepet = conf["MQTTRepet"];
  MQTTIP = conf["MQTTIP"];
  MQTTPort = conf["MQTTPort"];
  MQTTUser = conf["MQTTUser"].as<String>();
  MQTTPwd = conf["MQTTPwd"].as<String>();
  MQTTPrefix = conf["MQTTPrefix"].as<String>();
  MQTTPrefixEtat = conf["MQTTPrefixEtat"].as<String>();
  MQTTdeviceName = conf["MQTTdeviceName"].as<String>();
  TopicP = conf["TopicP"].as<String>();
  subMQTT = conf["subMQTT"];
  nomRouteur = conf["nomRouteur"].as<String>();
  nomSondeFixe = conf["nomSondeFixe"].as<String>();
  nomSfixePpos = conf["nomSfixePpos"] | nomSfixePpos;
  nomSfixePneg = conf["nomSfixePneg"] | nomSfixePneg;
  nomSondeMobile = conf["nomSondeMobile"].as<String>();
  for (int i = 1; i < LES_ROUTEURS_MAX; i++) {
    RMS_IP[i] = conf["RMS_IP" + String(i)];
  }
  for (int c = 0; c < 4; c++) {
    nomTemperature[c] = conf["nomTemperature" + String(c)].as<String>();
    Source_Temp[c] = conf["Source_Temp" + String(c)].as<String>();
    refTempIP[c] = conf["refTempIP" + String(c)];
    TopicT[c] = conf["TopicT" + String(c)].as<String>();
    canalTempExterne[c] = conf["canalTempExterne" + String(c)];
    offsetTemp[c] = conf["offsetTemp" + String(c)];
  }
  CalibU = conf["CalibU"];
  CalibI = conf["CalibI"];
  TempoRTEon = conf["TempoRTEon"];
  WifiSleep = conf["WifiSleep"];
  ComSurv = conf["ComSurv"];
  pSerial = conf["pSerial"];
  Serial2V = conf["Serial2V"].isNull() ? Serial2V : conf["Serial2V"];
  pTriac = conf["pTriac"];
  // Zone des actions
  ReacCACSI = conf["ReacCACSI"];
  Fpwm = conf["Fpwm"];
  NbActions = conf["NbActions"];
  int iAct = 0;
  JsonArray arr = conf["Actions"];
  for (JsonObject obj : arr) {
    if (iAct >= NbActions) break;  // éviter de dépasser le tableau
    LesActions[iAct].Actif = obj["Actif"];
    LesActions[iAct].Titre = obj["Titre"].as<String>();
    LesActions[iAct].Host = obj["Host"].as<String>();
    LesActions[iAct].Port = obj["Port"];
    LesActions[iAct].OrdreOn = obj["OrdreOn"].as<String>();
    LesActions[iAct].OrdreOff = obj["OrdreOff"].as<String>();
    LesActions[iAct].Repet = obj["Repet"];
    LesActions[iAct].Tempo = obj["Tempo"];

    if (!obj["Reactivite"].isNull()) {  //Ancien nom de variable en V15
      LesActions[iAct].Ki = obj["Reactivite"];
    } else {
      LesActions[iAct].Ki = obj["Ki"];
      LesActions[iAct].Kp = obj["Kp"];
      LesActions[iAct].Kd = obj["Kd"];
      LesActions[iAct].PID = obj["PID"];
    }
    LesActions[iAct].ForceOuvre = !obj["ForceOuvre"].isNull() ? obj["ForceOuvre"] : 100;
    LesActions[iAct].NbPeriode = obj["NbPeriode"];

    Hdeb = 0;
    JsonArray arrP = !obj["Périodes"].isNull() ? obj["Périodes"] : obj["Periodes"];  //Anciens fichiers avec accent à éviter
    byte i = 0;
    for (JsonObject objP : arrP) {
      if (i >= LesActions[iAct].NbPeriode) break;  // éviter de dépasser le tableau
      LesActions[iAct].Type[i] = objP["Type"];
      LesActions[iAct].Hfin[i] = objP["Hfin"];
      LesActions[iAct].Hdeb[i] = Hdeb;
      Hdeb = LesActions[iAct].Hfin[i];
      LesActions[iAct].Vmin[i] = objP["Vmin"];
      LesActions[iAct].Vmax[i] = objP["Vmax"];
      LesActions[iAct].ONouvre[i] = !objP["ONouvre"].isNull() ? objP["ONouvre"] : 100;
      LesActions[iAct].Tinf[i] = objP["Tinf"];
      LesActions[iAct].Tsup[i] = objP["Tsup"];
      LesActions[iAct].Hmin[i] = objP["Hmin"];
      LesActions[iAct].Hmax[i] = objP["Hmax"];
      LesActions[iAct].CanalTemp[i] = objP["CanalTemp"] | 0;
      LesActions[iAct].SelAct[i] = objP["SelAct"] | 0;
      LesActions[iAct].Ooff[i] = objP["Ooff"] | 0;
      LesActions[iAct].O_on[i] = objP["O_on"] | 0;
      LesActions[iAct].Tarif[i] = objP["Tarif"] | 0;
      i++;
    }
    iAct++;
  }
  if(Versioncompile != VersionStocke) RecordFichierParametres(); //Mise à jour num version
}

String SerializeConfiguration() {
  JsonDocument conf;

  byte NbPeriode;
  conf["Routeur"] = "F1ATB";
  String V = Version;
  int VersionStocke = round(100 * V.toFloat());
  conf["VersionStocke"] = VersionStocke;
  conf["ssid"] = ssid;
  conf["password"] = password;
  conf["dhcpOn"] = dhcpOn;
  conf["IP_Fixe"] = RMS_IP[0];
  conf["Gateway"] = Gateway;
  conf["masque"] = masque;
  conf["dns"] = dns;
  conf["hostname"] = hostname;
  conf["CleAccesRef"] = CleAccesRef;
  conf["hostname"] = hostname;
  conf["Couleurs"] = Couleurs;
  conf["ModePara"] = ModePara;
  conf["ModeReseau"] = ModeReseau;
  conf["Horloge"] = Horloge;
  conf["idxFuseau"] = idxFuseau;
  conf["ntpServer"] = ntpServer;
  conf["ESP32_Type"] = ESP32_Type;
  conf["LEDgroupe"] = LEDgroupe;
  conf["rotation"] = rotation;
  conf["DurEcran"] = DurEcran;
  conf["clickPresence"] = clickPresence;
  conf["NumPageBoot"] = NumPageBoot;
  for (int i = 0; i < 8; i++) {
    conf["Calibre" + String(i)] = Calibre[i];
  }
  conf["pUxI"] = pUxI;
  conf["pTemp"] = pTemp;
  conf["Source"] = Source;
  conf["RMSextIP"] = RMSextIP;
  conf["EnphaseUser"] = EnphaseUser;
  conf["EnphasePwd"] = EnphasePwd;
  conf["EnphaseSerial"] = EnphaseSerial;
  if (ModePara == 0) {
    MQTTRepet = 0;
    subMQTT = 0;
  }
  conf["MQTTRepet"] = MQTTRepet;
  conf["MQTTIP"] = MQTTIP;
  conf["MQTTPort"] = MQTTPort;
  conf["MQTTUser"] = MQTTUser;
  conf["MQTTPwd"] = MQTTPwd;
  conf["MQTTPrefix"] = MQTTPrefix;
  conf["MQTTPrefixEtat"] = MQTTPrefixEtat;
  conf["MQTTdeviceName"] = MQTTdeviceName;
  conf["TopicP"] = TopicP;
  conf["subMQTT"] = subMQTT;
  conf["nomRouteur"] = nomRouteur;
  conf["nomSondeFixe"] = nomSondeFixe;
  conf["nomSfixePpos"] = nomSfixePpos;
  conf["nomSfixePneg"] = nomSfixePneg;
  conf["nomSondeMobile"] = nomSondeMobile;
  for (int i = 0; i < LES_ROUTEURS_MAX; i++) {
    conf["RMS_IP" + String(i)] = RMS_IP[i];
  }

  for (int c = 0; c < 4; c++) {
    conf["nomTemperature" + String(c)] = nomTemperature[c];
    conf["Source_Temp" + String(c)] = Source_Temp[c];
    conf["refTempIP" + String(c)] = refTempIP[c];
    conf["TopicT" + String(c)] = TopicT[c];
    conf["canalTempExterne" + String(c)] = canalTempExterne[c];
    conf["offsetTemp" + String(c)] = offsetTemp[c];
  }
  conf["CalibU"] = CalibU;
  conf["CalibI"] = CalibI;
  conf["TempoRTEon"] = TempoRTEon;
  conf["WifiSleep"] = WifiSleep;
  conf["ComSurv"] = ComSurv;
  conf["pSerial"] = pSerial;
  conf["Serial2V"] = Serial2V;
  conf["pTriac"] = pTriac;
  // Enregistrement des Actions
  if (ReacCACSI < 1)
    ReacCACSI = 1;
  if (Fpwm < 5)
    Fpwm = 500;
  conf["ReacCACSI"] = ReacCACSI;
  conf["Fpwm"] = Fpwm;
  conf["NbActions"] = NbActions;
  JsonArray arr = conf["Actions"].to<JsonArray>();
  for (int iAct = 0; iAct < NbActions; iAct++) {

    JsonObject obj = arr.add<JsonObject>();
    obj["Action"] = iAct;
    obj["Actif"] = LesActions[iAct].Actif;
    obj["Titre"] = LesActions[iAct].Titre;
    obj["Host"] = LesActions[iAct].Host;
    obj["Port"] = LesActions[iAct].Port;
    obj["OrdreOn"] = LesActions[iAct].OrdreOn;
    obj["OrdreOff"] = LesActions[iAct].OrdreOff;
    obj["ForceOuvre"] = LesActions[iAct].ForceOuvre;
    obj["Repet"] = LesActions[iAct].Repet;
    obj["Tempo"] = LesActions[iAct].Tempo;
    obj["Kp"] = LesActions[iAct].Kp;
    obj["Ki"] = LesActions[iAct].Ki;
    obj["Kd"] = LesActions[iAct].Kd;
    obj["PID"] = LesActions[iAct].PID;
    obj["NbPeriode"] = LesActions[iAct].NbPeriode;
    //JsonArray arrP = obj["Périodes"].to<JsonArray>();
    JsonArray arrP = obj["Periodes"].to<JsonArray>();
    for (byte i = 0; i < LesActions[iAct].NbPeriode; i++) {
      JsonObject objP = arrP.add<JsonObject>();
      if (ModePara == 0) {  // standard
        LesActions[iAct].CanalTemp[i] = -1;
        LesActions[iAct].SelAct[i] = 255;
      }
      objP["Periode"] = i;
      objP["Type"] = LesActions[iAct].Type[i];
      objP["Hfin"] = LesActions[iAct].Hfin[i];
      objP["Vmin"] = LesActions[iAct].Vmin[i];
      objP["Vmax"] = LesActions[iAct].Vmax[i];
      objP["ONouvre"] = LesActions[iAct].ONouvre[i];
      objP["Tinf"] = LesActions[iAct].Tinf[i];
      objP["Tsup"] = LesActions[iAct].Tsup[i];
      objP["Hmin"] = LesActions[iAct].Hmin[i];
      objP["Hmax"] = LesActions[iAct].Hmax[i];
      objP["CanalTemp"] = LesActions[iAct].CanalTemp[i];
      objP["SelAct"] = LesActions[iAct].SelAct[i];
      objP["Ooff"] = LesActions[iAct].Ooff[i];
      objP["O_on"] = LesActions[iAct].O_on[i];
      objP["Tarif"] = LesActions[iAct].Tarif[i];
    }
  }
  String Json;
  serializeJson(conf, Json);
  return Json;
}

void Record_Data(String dateAMJ, String MesSage, int16_t HeureCouranteDeci_) {
  if (dateAMJ == "") return;
  //Test s'il y a de la place
  int Occupation = 100 * LittleFS.usedBytes() / LittleFS.totalBytes();
  Serial.println("Occupation:" + String(Occupation));
  if (Occupation > 80) {
    int LePlusVieux = 333333;
    int p, dateJ;
    String S = "", FileName;
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while (file) {
      FileName = String(file.name());
      p = FileName.indexOf(".csv");
      if (p > 6) {
        dateJ = FileName.substring(p - 6, p).toInt();
        if (dateJ < LePlusVieux) {
          LePlusVieux = dateJ;
          S = FileName;
        }
      }
      file = root.openNextFile();
    }
    LittleFS.remove("/" + S);  //On retire le fichier du mois le plus vieux
  }
  String AM_file = "/Mois_Wh_" + dateAMJ.substring(0, 6) + ".csv";
  bool biSonde = false;
  if (nomSondeFixe != "" && (Source_data == "UxIx2" || ((Source_data == "ShellyEm" || Source_data == "ShellyPro") && EnphaseSerial.toInt() != 3))) biSonde = true;

  String New_Record_Conf = "Date";
  String Data = dateAMJ + "," + String(EnergieJour_M_Soutiree) + "," + String(EnergieJour_M_Injectee);
  if (EnergieJour_M_Soutiree>1000000 || EnergieJour_M_Injectee>1000000) Data = dateAMJ + ", ," ; //Aberration à certains ReseT
  New_Record_Conf += "," + Filtre_Nom(nomSondeMobile) + " / Soutirée," + Filtre_Nom(nomSondeMobile) + " / Injectée";
  if (nomSondeFixe != "" && nomSfixePpos != "" && biSonde && EnergieJour_T_Soutiree<1000000) {
    New_Record_Conf += "," + Filtre_Nom(nomSondeFixe) + " / " + Filtre_Nom(nomSfixePpos);
    Data += "," + String(EnergieJour_T_Soutiree);
  } else {
    New_Record_Conf += ",";  //Colonne vide
    Data += ",";
  }
  if (nomSondeFixe != "" && nomSfixePneg != "" && biSonde && EnergieJour_T_Injectee<1000000) {
    New_Record_Conf += "," + Filtre_Nom(nomSondeFixe) + " / " + Filtre_Nom(nomSfixePneg);
    Data += "," + String(EnergieJour_T_Injectee);
  } else {
    New_Record_Conf += ",";  //Colonne vide
    Data += ",";
  }
  New_Record_Conf += ",Heure Déci. ,Message";
  float Duree = float(HeureCouranteDeci_) / 100.0;
  Data += "," + String(Duree) + "," + MesSage;
  if (!LittleFS.exists(AM_file)) {
    File file = LittleFS.open(AM_file, FILE_WRITE);
    file.print(New_Record_Conf + "\r\n");
    file.close();
  } else {

    if (New_Record_Conf != Record_Conf) {  // Nouvelle configuration
      Record_Conf = New_Record_Conf;
      File fileJ = LittleFS.open(AM_file, FILE_APPEND);
      fileJ.print(Record_Conf + "\r\n");
      fileJ.close();
    }
  }
  File fileJ = LittleFS.open(AM_file, FILE_APPEND);
  fileJ.print(Data + "\r\n");
  fileJ.close();
}
String Filtre_Nom(String Nom) {
  Nom.replace(",", ".");
  Nom.replace("#", "=");
  return Nom;
}
void LitLastRecord_Conf() {  // Lire la dernière configuration connue

  String ligne = "";
  LastRecordConf = true;
  String AM_file = "/Mois_Wh_" + DateAMJ.substring(0, 6) + ".csv";

  if (LittleFS.exists(AM_file)) {
    File file = LittleFS.open(AM_file);
    while (file.available()) {
      char c = file.read();
      if (c == '\n' || c == '\r') {
        if (ligne.length() > 10) {
          if (ligne.indexOf("Date,") == 0) {
            ligne.trim();
            Record_Conf = ligne;
          }
          ligne = "";
        }
      } else {
        ligne += String(c);
      }
    }
    file.close();
  } else {
    Record_Conf = "";
  }
}
