void SplitS(String Str, String &Before, String Separ, String &After) {
  int p = Str.indexOf(Separ);
  Before = Str.substring(0, p);
  After = Str.substring(p + 1);
}
// Conversion des adresses IP suivant le coeur

String IP2String(unsigned long IP) {
  byte arr[4];
  arr[0] = IP & 0xFF;            // 0x78
  arr[1] = (IP >> 8) & 0xFF;     // 0x56
  arr[2] = (IP >> 16) & 0xFF;    // 0x34
  arr[3] = (IP >> 24) & 0xFF;    // 0x12
  for (int i = 0; i < 4; i++) {  //Pour eviter corruption données entre coeur 0 et coeur1 (arr en variable local)
    arrIP[i] = arr[i];           //Pour le WIFI au setup
  }
  return String(arr[3]) + "." + String(arr[2]) + "." + String(arr[1]) + "." + String(arr[0]);
}
unsigned long String2IP(String S) {
  unsigned long IP = 0;
  for (int i = 0; i < 3; i++) {
    int p = S.indexOf(".");
    String s = S.substring(0, p);
    S = S.substring(p + 1);
    IP += s.toInt();
    IP = IP * 256;
  }
  IP += S.toInt();
  return IP;
}

//Gestion couleurs
String ULtoHex(unsigned long x) {
  char buffer[15];
  ltoa(x, buffer, 16);
  String S = "000000" + String(buffer);
  int p = S.length();
  S = "#" + S.substring(p - 6);  //Format pour HTML color
  return S;
}
unsigned long ConvCouleur(String V) {  //Notation CSS en UL
  return strtoul(V.c_str(), NULL, 16);
}

//Telnet et Serial
//****************

// --- Fonction de sortie partagée ---
void TelnetPrint(const String &message) {
  Serial.print(message);  // Sortie sur port série
  if (telnetClient && telnetClient.connected() && TelnetOn) {
    telnetClient.print(message);  // Sortie sur Telnet
  }
}
void TelnetPrintln(const String &message) {
  Serial.println(message);
  if (telnetClient && telnetClient.connected() && TelnetOn) {
    telnetClient.println(message);
  }
}
// PORT SERIE ou TELNET
void LireSerial() {
  int inbyte;
  //Port Serie
  while (Serial.available() > 0) {
    inbyte = Serial.read();


    if ((inbyte == 10) || (inbyte == 13)) {
      DecodeSerial();
    } else {
      SerialIn += String(char(inbyte));
    }
  }
  //Telnet
  if (telnetClient && telnetClient.connected() && telnetClient.available() && TelnetOn) {
    inbyte = telnetClient.read();  // Lire caractère

    if ((inbyte == 10) || (inbyte == 13)) {  // Fin de ligne → commande complète
      DecodeSerial();
    } else {
      if (inbyte >= 32) {
        SerialIn += String(char(inbyte));  // Ajouter caractère au buffer
      }
    }
  }
}
void DecodeSerial() {
  String sw;
  String valeur = "";
  int p;
  SerialIn.trim();

  p = SerialIn.indexOf(":");
  if (p > 0) {
    sw = SerialIn.substring(0, p + 1);
    valeur = SerialIn.substring(p + 1);
    sw.trim();
    valeur.trim();
  } else {
    sw = SerialIn;
  }

  if (sw.indexOf("restart") >= 0) {
    ESP.restart();
  }
  if (sw.indexOf("ssid:") >= 0) {
    ssid = valeur;
    ModeReseau = 0;          // A priori
    dhcpOn = 1;              //Au cas ou l'on change de mapping des adresses LAN
    if (ESP32_Type >= 10) {  //Carte Ethernet
      ESP32_Type = 0;
    }
    EcritureEnROM();
  }
  if (sw.indexOf("password:") >= 0) {
    password = valeur;
    EcritureEnROM();
  }
  if (sw.indexOf("ETH01") >= 0) {
    ESP32_Type = 10;
    EcritureEnROM();
  }
  if (sw.indexOf("dispPw") >= 0) {
    dispPw = !dispPw;
  }
  if (sw.indexOf("dispAct") >= 0) {
    dispAct = !dispAct;
  }

  if (sw.indexOf("P:") >= 0 || sw.indexOf("T:") >= 0) {
    if (sw.indexOf("T:") >= 0) {
      testTrame = valeur.toInt();
    }
    if (sw.indexOf("P:") >= 0) {
      testPulse = valeur.toInt();
    }
    if (testPulse > testTrame) testPulse = testTrame;
    TelnetPrintln("Rapport cyclique Pulse " + String(testPulse * 10) + " ms / Trame " + String(testTrame * 10) + " ms  : P=" + String(testPulse) + "  T=" + String(testTrame));
  }
  if ((sw.indexOf("H")>=0 || sw.indexOf("?")>=0 ) && p==-1){
    MessageCommandes();
  }
  if (SerialIn!="") TelnetPrintln(">>" + SerialIn);
  SerialIn = "";
}
// commandes disponibles par port serie ou Telnet
const char *helpSerial = R"====(
**************
commandes pour configuration par port série ou Telnet (respect majuscules, ponctuation et terminer par touche 'Enter'):

ssid:xxx     | Pour définir le nom xxx du Wifi à utiliser
password:yyy | Pour définir le mot de passe yyy du Wifi
restart      | Pour redémarrer l'ESP32 sans appui sur le bouton EN
dispPw       | Pour afficher les mesures de puissance Pw
dispAct      | Pour afficher les ouvertures des Actions
ETH01        | Bascule sur la config ethernet avec bus RMII et LAN8720
T:xxx        | En mode Train de Sinus force la longeur de Trame. 
             | T:xxx = xxx*10ms, T:0 retourne en Train de sinus normal
P:yyy        | En mode Train de Sinus force la longueur des Pulses. 
             | P:yyy = yyy*10ms. P<=T . Evitez P impaire et Trame paire. 
H ou ?       | pour avoir cette aide
**************
)====";


void MessageCommandes() {
  String M = helpSerial;
  String Before;
  while (M.indexOf("\n") >= 0) {
    SplitS(M, Before, "\n", M);
    TelnetPrintln(Before);
  }
}