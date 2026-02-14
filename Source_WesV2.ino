// ****************************************************
// * Client WES v2 Cartelectronic - Pinces Ampèremétriques *
// ****************************************************

// Constantes pour les timeouts
const unsigned long WES_CONNECT_TIMEOUT = 3000;  // Timeout de connexion en ms
const unsigned long WES_READ_TIMEOUT = 5000;     // Timeout de lecture en ms

// Fonction pour extraire une valeur XML simple
// Exemple: ValXml("I1", xmlData) retourne "2.96" depuis "<I1>2.96</I1>"
float ValXml(String nom, String Xml) {
  String baliseDebut = "<" + nom + ">";
  String baliseFin = "</" + nom + ">";
  int p1 = Xml.indexOf(baliseDebut);
  if (p1 < 0)
    return 0;

  p1 += baliseDebut.length();
  int p2 = Xml.indexOf(baliseFin, p1);
  if (p2 < 0)
    return 0;

  String valeur = Xml.substring(p1, p2);
  valeur.trim();
  return valeur.toFloat();
}

// Fonction pour encoder en Base64 (pour HTTP Basic Auth)
String base64Encode(String str) {
  const char *base64_chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  String encoded = "";
  int val = 0;
  int valb = -6;

  for (unsigned char c : str) {
    val = (val << 8) + c;
    valb += 8;
    while (valb >= 0) {
      encoded += base64_chars[(val >> valb) & 0x3F];
      valb -= 6;
    }
  }

  if (valb > -6) {
    encoded += base64_chars[((val << 8) >> (valb + 8)) & 0x3F];
  }

  while (encoded.length() % 4) {
    encoded += '=';
  }

  return encoded;
}

void LectureWesV2() {
  String Wes_Data = "";
  float current = 0;      // Intensité en Ampères
  float voltage = 0;      // Tension en Volts
  float cosPhi = 0;       // Facteur de puissance
  float energyIndex = 0;  // Index énergie totale en kWh
  float energyInject = 0; // Index énergie injectée en kWh

  // Connexion HTTP au WES v2
  WiFiClient clientESP_RMS;
  String host = IP2String(RMSextIP);

  if (!clientESP_RMS.connect(host.c_str(), 80, WES_CONNECT_TIMEOUT)) {
    delay(500);
    if (!clientESP_RMS.connect(host.c_str(), 80, WES_CONNECT_TIMEOUT)) {
      delay(100);
      clientESP_RMS.stop();
      StockMessage("WES v2: Connection failed to " + host + " - Check IP and network");
      return;
    }
  }

  // Préparation de l'authentification HTTP Basic
  String auth = WesUser + ":" + WesPwd;
  String authEncoded = base64Encode(auth);

  // Requête HTTP GET vers data.cgx
  String url = "/data.cgx";
  clientESP_RMS.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host +
                      "\r\n" + "Authorization: Basic " + authEncoded + "\r\n" +
                      "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (clientESP_RMS.available() == 0) {
    if (millis() - timeout > WES_READ_TIMEOUT) {
      StockMessage("WES v2: Timeout waiting for response from " + host);
      clientESP_RMS.stop();
      delay(100);
      return;
    }
  }

  // Lecture de la première ligne (status HTTP)
  timeout = millis();
  String statusLine = clientESP_RMS.readStringUntil('\n');
  
  // Validation du code de statut HTTP
  if (statusLine.indexOf("200") < 0) {
    if (statusLine.indexOf("401") >= 0) {
      StockMessage("WES v2: Authentication failed - Check username and password");
    } else if (statusLine.indexOf("404") >= 0) {
      StockMessage("WES v2: File not found (404) - Check WES configuration");
    } else {
      StockMessage("WES v2: HTTP error - " + statusLine);
    }
    clientESP_RMS.stop();
    return;
  }

  // Lecture de la réponse HTTP
  timeout = millis();
  bool headersEnded = false;
  while (clientESP_RMS.available() && (millis() - timeout < WES_READ_TIMEOUT)) {
    String line = clientESP_RMS.readStringUntil('\n');

    // Détection de la fin des headers HTTP (ligne vide)
    if (!headersEnded) {
      if (line == "\r" || line == "") {
        headersEnded = true;
      }
      continue;
    }

    // Lecture du corps XML
    Wes_Data += line;
  }
  clientESP_RMS.stop();

  // Validation du numéro de pince
  byte pinceNum = WesPinceNum;
  if (pinceNum < 1 || pinceNum > 8) pinceNum = 1;  // Validation: pince 1-8

  // Stockage des données brutes pour debug
  Wes_dataBrute = "<strong>WES v2 - Pince " + String(pinceNum) +
                    "</strong><br>" + Wes_Data;

  // Extraction des données de la pince sélectionnée
  String pinceTag = String(pinceNum);
  
  // On ne garde que la partie <pince>...</pince> pour éviter de lire les <INDEXn> de la partie <impulsion>
  int pStart = Wes_Data.indexOf("<pince>");
  if (pStart > 0) {
    Wes_Data = Wes_Data.substring(pStart);
  }

  // Lecture de la tension (commune à toutes les pinces)
  voltage = ValXml("V", Wes_Data);

  // Validation de la tension pour éviter les trous de valeurs (0V si parsing échoué)
  if (voltage < 90.0) {
    return;
  }

  // Lecture des données spécifiques à la pince
  current = ValXml("I" + pinceTag, Wes_Data);
  cosPhi = ValXml("COSPHI" + pinceTag, Wes_Data);
  energyIndex = ValXml("INDEX" + pinceTag, Wes_Data);
  energyInject = ValXml("IDXINJECT" + pinceTag, Wes_Data);

  // Calcul de la puissance active : P = V * I * |cos(phi)|
  // Le signe de COSPHI indique le sens :
  // - COSPHI positif = consommation (soutirage)
  // - COSPHI négatif = production (injection)
  float Pva = voltage * current;   // Puissance apparente S = V * I
  float PwAbs = Pva * abs(cosPhi); // Puissance active P = S * |cos(phi)|

  // Mise à jour des variables globales selon le signe de COSPHI
  if (cosPhi >= 0) {
    // COSPHI positif = Consommation
    PuissanceS_M_inst = PwAbs;
    PuissanceI_M_inst = 0;
    PVAS_M_inst = PfloatMax(Pva);
    PVAI_M_inst = 0;
  } else {
    // COSPHI négatif = Production (injection)
    PuissanceS_M_inst = 0;
    PuissanceI_M_inst = PwAbs;
    PVAI_M_inst = PfloatMax(Pva);
    PVAS_M_inst = 0;
  }

  // Mise à jour des autres paramètres
  Tension_M = voltage;
  Intensite_M = current;
  PowerFactor_M = abs(cosPhi);

  // Énergie cumulée (INDEX et IDXINJECT sont en kWh, on convertit en Wh)
  // INDEX = énergie soutirée (consommée)
  // IDXINJECT = énergie injectée (produite)
  energyIndex = ValXml("INDEX" + pinceTag, Wes_Data);
  energyInject = ValXml("IDXINJECT" + pinceTag, Wes_Data);
  
  Energie_M_Soutiree = int(energyIndex * 1000);
  Energie_M_Injectee = int(energyInject * 1000);

  // Validation des données
  Pva_valide = true;

  // Filtrage de la puissance (fonction existante du routeur)
  filtre_puissance();

  // Reset du Watchdog
  PuissanceRecue = true;
  EnergieActiveValide = true;

  // Reset LED jaune (indicateur de communication)
  if (cptLEDyellow > 30) {
    cptLEDyellow = 4;
  }
}
