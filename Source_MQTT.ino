
// ******************************************************
// * Informations de puissance reçue via un Broker MQTT *
// ******************************************************
// Pmqtt peut être utilisée comme Source_M (topic = TopicP) ou Source_T (topic = TopicP_T).
// Les valeurs reçues sont mises en cache dans deux jeux distincts (PwMQTT/PwMQTT_T, etc.)
// par le callback MQTT (voir EnvoiMQTT.ino). UpdatePmqtt() choisit le bon jeu selon
// le canal de lecture courant et écrit le résultat via les pointeurs canal.
void UpdatePmqtt() {
  // Sélectionne la source de vérité (M ou T) selon le canal courant
  float Pw_raw, Pva_raw, Pf_raw;
  String Brute;
  unsigned long lastReceived;
  if (canal_lecture_courant == 'T') {
    Pw_raw  = PwMQTT_T;
    Pva_raw = PvaMQTT_T;
    Pf_raw  = PfMQTT_T;
    Brute   = P_MQTT_Brute_T;
    lastReceived = LastPwMQTTMillis_T;
  } else {
    Pw_raw  = PwMQTT;
    Pva_raw = PvaMQTT;
    Pf_raw  = PfMQTT;
    Brute   = P_MQTT_Brute;
    lastReceived = LastPwMQTTMillis;
  }

  float Pw = PfloatMax(Pw_raw);
  float Pf = 1;
  if (Brute.indexOf("Pf") > 0) {
    Pf = abs(Pf_raw);
  }
  if (Brute.indexOf("Pva") > 0) {
    if (Pva_raw != 0) {
      Pf = abs(Pw / PfloatMax(Pva_raw));
    }
  }
  if (Brute.indexOf("Pva") > 0 || Brute.indexOf("Pf") > 0) {
    Pva_valide = true;
  } else {
    Pva_valide = false;
  }
  if (Pf > 1) Pf = 1;

  // Écriture via les pointeurs canal (M ou T)
  if (Pw >= 0) {
    *pPuissanceS_inst = Pw;
    *pPuissanceI_inst = 0;
    if (Pf > 0.01) {
      *pPVAS_inst = PfloatMax(Pw / Pf);
    } else {
      *pPVAS_inst = 0;
    }
    *pPVAI_inst = 0;
    *pEASfloat += Pw / 6000.0;               // Watt Hour, every 600ms. Soutirée
    *pEnergie_Soutiree = long(*pEASfloat);
  } else {
    *pPuissanceS_inst = 0;
    *pPuissanceI_inst = -Pw;
    if (Pf > 0.01) {
      *pPVAI_inst = PfloatMax(-Pw / Pf);
    } else {
      *pPVAI_inst = 0;
    }
    *pPVAS_inst = 0;
    *pEAIfloat += -Pw / 6000.0;
    *pEnergie_Injectee = long(*pEAIfloat);
  }

  filtre_puissance();

  if (Brute.indexOf("Pw") > 0) EnergieActiveValide = true;
  // Le watchdog n'est rafraîchi que par la source M (qui pilote le routage)
  if (canal_lecture_courant == 'M' && millis() - lastReceived < 30000) PuissanceRecue = true;

  if (cptLEDyellow > 30) {
    cptLEDyellow = 4;
  }
}
