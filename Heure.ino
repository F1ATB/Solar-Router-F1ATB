// Heure et Date
#define MAX_SIZE_T 80

const char *ntpServer1 = "fr.pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";

unsigned short Int_Last_10Millis = 0;                                                                      // Europe Centrale                                                                                  //-1=inconnu,0=dimache,1=lundi...
String codeTZ[] = { "CET-1CEST,M3.5.0,M10.5.0/3", "AST4", "GFT3", "RET-4", "EAT-3", "NCT-11", "WFT-12" };  // Europe centrale, Guadeloupe / Martinique, Guyane, Réunion, Mayotte,Nouvelle Calédonie, Wallis et Futuna
int8_t Jour = -1;                                                                                          //-1=inconnu,0=dimanche,1=lundi...
uint64_t baseTick10ms = 0;
time_t baseEpoch = 0;
void FormatteHeureDate() {
  // Formatte la date et l'heure en String
  char buffer[MAX_SIZE_T];
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);
  strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &timeinfo);
  DATE = String(buffer);
  strftime(buffer, sizeof(buffer), "%Y%m%d", &timeinfo);
  DateAMJ = String(buffer);
  Int_Heure = timeinfo.tm_hour;
  Int_Minute = timeinfo.tm_min;
  Jour = timeinfo.tm_wday;  //-1=inconnu,0=dimanche,1=lundi...
  HeureValide = true;
  if ( !LastRecordConf)  LitLastRecord_Conf(); 
}

void JourHeureChange() {
  T_On_seconde = esp_timer_get_time() / 1000000;
  if (Horloge == 3 || Horloge == 4) {
    unsigned short Tnow = CptIT;
    if (Horloge == 4 && ITmode > 0) {  // Horloge sur IT 20ms
      StepIT = 2;
    } else {
      StepIT = 1;
    }
    unsigned short deltaT = Tnow - Int_Last_10Millis;
    while (deltaT >= 100) {
      Int_Last_10Millis = Int_Last_10Millis + 100;
      baseEpoch++;  // 100 ticks = 1 seconde
    }
    struct timeval tv = { .tv_sec = baseEpoch, .tv_usec = 0 };
    settimeofday(&tv, nullptr);
  }


  if (HeureValide) {
    FormatteHeureDate();
    HeureCouranteDeci = Int_Heure * 100 + Int_Minute * 10 / 6;
    if (old_Heure == 5 && Int_Heure == 6) {
      for (int i = 0; i < LES_ACTIONS_LENGTH; i++) {
        LesActions[i].H_Ouvre = 0;  //RAZ temps equivalent ouverture à 6h du matin
      }
    }
    if (old_Heure == 23 && Int_Heure == 0) {
      erreurTriac = false;
      if (EnergieActiveValide) {  //Données recues
        int16_t old_HeureCouranteDeci = old_Heure * 100 + old_Minute * 10 / 6;
        Record_Data(oldDateAMJ,oldDateAMJ,old_HeureCouranteDeci);
        idxPromDuJour = (idxPromDuJour + 1 + NbJour) % NbJour;
        //On enregistre les conso en début de journée pour l'historique de l'année
        long energie = Energie_M_Soutiree - Energie_M_Injectee;  //Bilan energie du jour
        EEPROM.writeLong(idxPromDuJour * 4, energie);
        EEPROM.writeULong(adr_E_T_soutire0, long(Energie_T_Soutiree));
        EEPROM.writeULong(adr_E_T_injecte0, long(Energie_T_Injectee));
        EEPROM.writeULong(adr_E_M_soutire0, long(Energie_M_Soutiree));
        EEPROM.writeULong(adr_E_M_injecte0, long(Energie_M_Injectee));
        EEPROM.writeString(adr_DateCeJour, "");
        EEPROM.writeUShort(adr_lastStockConso, idxPromDuJour);
        EEPROM.commit();
        LectureConsoMatinJour();
        
      }
      //Puissance Max du jour à zero
      PuisMaxS_T = 0;
      PuisMaxS_M = 0;
      PuisMaxI_T = 0;
      PuisMaxI_M = 0;
    }
    old_Heure = Int_Heure;
    old_Minute = Int_Minute;
    oldDateAMJ = DateAMJ;
  }
}
// **************
// * Heure DATE * -
// **************

void InitHeure() {
  if (Horloge == 0) {  //heure par Internet}
    //Heure / Hour . A Mettre en priorité avant WIFI (exemple ESP32 Simple Time)

    sntp_set_sync_interval(10800000);  //Synchro toutes les 3h
    sntp_set_time_sync_notification_cb(time_sync_notification);
    //sntp_servermode_dhcp(1);   Déprecié
    esp_sntp_servermode_dhcp(true);
    if (ntpServer != "") {
      char buffer[30];
      ntpServer.toCharArray(buffer, sizeof(buffer));
      const char* NTP = buffer;

      configTzTime(codeTZ[idxFuseau].c_str(), NTP,ntpServer1, ntpServer2);  //Voir Time-Zone:
    } else { 
      Serial.println("Heure Defaut:") ;                                                                               //Option
      configTzTime(codeTZ[idxFuseau].c_str(), ntpServer1, ntpServer2);  //Voir Time-Zone:
    }
    Serial.println("idxFuseau: " +String(idxFuseau));
    Serial.println("CODE: " +codeTZ[idxFuseau]);
  }
}

void MiseAheure(String New_H, String New_J) {
  if (Horloge >= 2 && Horloge <= 5) {
    int H_, Mn;
    New_H.trim();
    New_J.trim();
    New_H = "00" + New_H;
    int p = New_H.indexOf(":");
    if (p > 0) {
      H_ = (New_H.substring(p - 2, p).toInt()) % 24;
      Mn = (New_H.substring(p + 1).toInt()) % 60;
    }
    if (New_J.substring(2, 3) == "/" && New_J.substring(5, 6) == "/") {
      struct tm t = { 0 };                                // toujours initialiser à 0
      t.tm_year = New_J.substring(6, 10).toInt() - 1900;  // années depuis 1900
      t.tm_mon = New_J.substring(3, 5).toInt() - 1;       // mois 0–11 (octobre = 9)
      t.tm_mday = New_J.substring(0, 2).toInt();          // jour du mois
      t.tm_hour = H_;
      t.tm_min = Mn;
      t.tm_sec = 0;

      baseEpoch = mktime(&t);
      //  Création d’une structure timeval pour settimeofday()
      struct timeval tv = { .tv_sec = baseEpoch, .tv_usec = 0 };
      settimeofday(&tv, nullptr);  // mise à l'heure de l’ESP32
      FormatteHeureDate();
    }
  }
}
void time_sync_notification(struct timeval *tv) {
  TelnetPrintln("\nNotification de l'heure ( time synchronization event ) ");
  TelnetPrint("Sync time in ms : ");
  TelnetPrintln(String(sntp_get_sync_interval()));
  FormatteHeureDate();
  JourHeureChange();
  StockMessage("Réception de l'heure Internet");
}
