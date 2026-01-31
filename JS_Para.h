//***************************************************
// Page Javascript  de gestion des Paramètres
//***************************************************
const char * ParaJS1 = R"====(
var refTempIP = [];
var BordsInverse = [".Bparametres", ".Bgeneraux"];
/**
 * Initialise les éléments HTML dynamiques pour les sources de température et les routeurs.
 */
function Init() {
    let S = "";

    // --- Génération des sources de température (4 canaux) ---
    const NbCanauxTemp = 4;
    for (let i = 0; i < NbCanauxTemp; i++) {
        // Bloc 1: Sélection du type de capteur
        S += "<div class='form cadre'>";
        S += '<div class="ligne">';
        S += `<label for="srcTemp${i}">Canal : ${i}</label>`;
        S += `<select id="srcTemp${i}" onclick="checkDisabled();">
                  <option value="tempNo" selected>Pas de mesure</option>
                  <option value="tempInt">Capteur DS18B20 Interne</option>
                  <option value="tempExt">Capteur DS18B20 ESP Externe</option>
                  <option value="tempMqtt">MQTT</option>
              </select>`;
        S += '</div>';
        S += "</div>";

        // Bloc 2: Paramètres spécifiques
        S += `<div class='form ligneTemperature' id='ligneTemperature${i}'>`;
        S += "<div class='ligne'>";
        S += `<label for='nomTemperature${i}'>Nom Température : </label>`;
        S += `<input type='text' name='nomTemperature${i}' id='nomTemperature${i}'>`;
        S += "</div>";
        S += `<div class='ligne' id='ligneoffsetTemp${i}'>`;
        S += `<label for='offsetTemp${i}'>Offset mesure température (°C) :</label>`;
        S += `<input type='number' name='offsetTemp${i}' id='offsetTemp${i}'>`;
        S += "</div>";
        S += `<div class='ligne' id='ligneTopicT${i}'>`;
        S += `<label for='TopicT${i}'>MQTT Topic Température :</label>`;
        S += `<input type='text' name='TopicT${i}' id='TopicT${i}'>`;
        S += "</div>";
        S += `<div class='ligne' id='ligneIPtemp${i}'>`;
        S += `<label for='refTempIP${i}'>Adresse IP ESP-RMS externe :</label>`;
        // Reste vide, rempli par LoadParaRouteur
        S += `<select id='refTempIP${i}'></select>`;
        S += "</div>";
        S += `<div class='ligne' id='canalEXTtemp${i}'>`;
        S += `<label for='canalEXT${i}'>Canal Température ESP-RMS externe :</label>`;
        S += `<select id='canalEXT${i}'>
                  <option value="0">Canal 0</option>
                  <option value="1">Canal 1</option>
                  <option value="2">Canal 2</option>
                  <option value="3">Canal 3</option>
              </select>`;
        S += "</div>";
        S += "</div>";
    }

    GH("Sources_Temp", S);

    // --- Génération des routeurs/RMS externes (8 entrées) ---
    S = "<div class='ligne'><div class='nomR'><strong>Nom</strong></div><div class='nomR'><strong>Adresse IP</strong></div></div>";
    const NbRouteurs = 8;
    for (let i = 0; i < NbRouteurs; i++) {
        // La ligne est initialement masquée avec 'display:none;'
        S += `<div class='ligne' id='Routeur_${i}' style='display:none;'>`;
        let bg = (i === 0) ? "style='background-color:#bbb;'" : ""; // Fond gris pour le premier élément
        S += `<div id='RMS_Nom${i}' class='nomR'></div><input type='text' id='RMS_IP${i}' ${bg} title='adresse au format IPv4 :xxx.xxx.xxx.xxx'>`;
        S += "</div>";
    }
    GH("Routeurs", S);

    // Chargement des données initiales
    SetHautBas();
    LoadParaFixe()
    
}

/**
 * Implémente les  paramètres après chargement depuis le serveur via AJAX.
 */
function SetParaFixe() {
    // Lecture du cookie de clé d'accès
    const cookieName = "CleAcces=";
    const ca = document.cookie.split(';');
    for (let i = 0; i < ca.length; i++) {
        let c = decodeURIComponent(ca[i]).trim();
        if (c.indexOf(cookieName) === 0) {
            GID("CleAcces").value = c.substring(cookieName.length);
            break; // Arrêter après avoir trouvé le cookie
        }
    }

   

    // --- Paramètres Généraux (Para) ---
    GID("dhcp").checked = F.dhcpOn == 1;
    GID("gateway").value =int2ip( F.Gateway);
    GID("masque").value = int2ip(F.masque);
    GID("dns").value = int2ip(F.dns);
    GID("hostname").value = F.hostname;

    // Mode de paramétrage
    GID("ModeP" + F.ModePara).checked = true;
 

    // Autres paramètres
    GID("ModeW").value = F.ModeReseau;
    GID("sources").value = F.Source;
    GID("RMSextIP").value = int2ip(F.RMSextIP);
    GID("EnphaseUser").value = F.EnphaseUser;
    GID("EnphasePwd").value = F.EnphasePwd;
    GID("EnphaseSerial").value = F.EnphaseSerial;
    GID("TopicP").value = F.TopicP;
    GID("MQTTRepet").value = F.MQTTRepet;
    GID("MQTTIP").value = int2ip(F.MQTTIP);
    GID("MQTTPort").value = F.MQTTPort;
    GID("MQTTUser").value = F.MQTTUser;
    GID("MQTTPwd").value = F.MQTTPwd;
    GID("MQTTPrefix").value = F.MQTTPrefix;
    GID("MQTTPrefixEtat").value = F.MQTTPrefixEtat;
    GID("MQTTdeviceName").value = F.MQTTdeviceName;
    GID("subMQTT").checked = F.subMQTT == 1;
    GID("nomRouteur").value = F.nomRouteur;
    GID("nomSondeFixe").value = F.nomSondeFixe;
    GID("nomSondeMobile").value = F.nomSondeMobile;
    GID("nomSfixePpos").value=F.nomSfixePpos;
    GID("nomSfixePneg").value=F.nomSfixePneg;
    GID("CalibU").value = F.CalibU;
    GID("CalibI").value = F.CalibI;
    GID("TempoRTEon").checked = F.TempoRTEon == 1;
    GID("WifiSleep").checked = F.WifiSleep == 1;
    GID("ComSurv").value = F.ComSurv;
    GID("Serie").value = F.pSerial;
    GID("Serial2V").value =F.Serial2V;
    GID("pTriac").value = F.pTriac;
    GID("ESP").value = F.ESP32_Type;
    GID("pLED").value = F.LEDgroupe;
    GID("Rot").value = F.rotation;
    GID("DurEcran").value = F.DurEcran;
    GID("clickPresence").value = F.clickPresence;
    GID("NumPageBoot").value = F.NumPageBoot;
    GID("pUxI").value = F.pUxI;
    GID("PTemp").value = F.pTemp;
    GID("EstimCACSI").checked = (F.ReacCACSI == 100)? true:false; 1;

    // --- Paramètres de Température (LesParas[1] à LesParas[4]) ---
    const NbCanauxTemp = 4;
   
    for (let c = 0; c < NbCanauxTemp; c++) {
        GID(`nomTemperature${c}` ).value = F[`nomTemperature${c}`];
        GID(`srcTemp${c}`).value = F[`Source_Temp${c}`];
        GID(`TopicT${c}`).value = F[`TopicT${c}`];
        // Stocke l'IP de référence dans le tableau global pour une utilisation ultérieure
        refTempIP[c] = F[`refTempIP${c}`];
        GID(`canalEXT${c}`).value = F[`canalTempExterne${c}`];
       GID(`offsetTemp${c}`).value = F[`offsetTemp${c}`]/100;
    }
    Set_Couleurs();
    LoadParaVar();
    GID("LesParas").style.display="block";

}

/**
 * Envoie les valeurs des paramètres au serveur via AJAX.
 */
function SendValues() {
  GID("attente").style = "visibility: visible;";
  // --- Récupération des valeurs  ---
  F.dhcpOn = GID("dhcp").checked ? 1 : 0;
  F.TempoRTEon = GID("TempoRTEon").checked ? 1 : 0;
  F.ComSurv = GID("ComSurv").value;
  F.Serial2V = GID("Serial2V").value;
  F.subMQTT = GID("subMQTT").checked ? 1 : 0;
  F.WifiSleep = GID("WifiSleep").checked ? 1 : 0;
  F.hostname =GID("hostname").value;
  F.pSerial = GID("Serie").value;
  F.pTriac = GID("pTriac").value;
  F.ModePara = document.querySelector('input[name="ModeP"]:checked').value;
  F.ModeReseau = GID("ModeW").value;
  F.ESP32_Type = GID("ESP").value;
  F.LEDgroupe = GID("pLED").value;
  F.rotation = GID("Rot").value;
  F.DurEcran = GID("DurEcran").value;
  F.clickPresence = GID("clickPresence").value;
  F.NumPageBoot = GID("NumPageBoot").value;
  F.pUxI = GID("pUxI").value;
  F.pTemp = GID("PTemp").value;
  F.ReacCACSI = GID("EstimCACSI").checked ? 100 : 0; 
  F.Source=GID("sources").value;
  if (F.ModePara == 0) { //Non Expert
    F.subMQTT = 0; F.WifiSleep = 1;
  }
  F.RMSextIP = ip2int(GID("RMSextIP").value);
  F.Gateway = ip2int(GID("gateway").value);
  F.masque = ip2int(GID("masque").value);
  F.dns = ip2int(GID("dns").value);

  F.nomRouteur =GID("nomRouteur").value.trim() ;
  F.nomSondeFixe = GID("nomSondeFixe").value.trim();
  F.nomSfixePpos = GID("nomSfixePpos").value.trim();
  F.nomSfixePneg = GID("nomSfixePneg").value.trim();
  F.nomSondeMobile = GID("nomSondeMobile").value.trim();

  F.MQTTRepet = GID("MQTTRepet").value;
  F.MQTTIP = ip2int(GID("MQTTIP").value);
  F.MQTTPort = GID("MQTTPort").value;
  F.MQTTUser = GID("MQTTUser").value.trim();
  F.MQTTPwd = GID("MQTTPwd").value.trim();
  F.MQTTPrefix = GID("MQTTPrefix").value.trim();
  F.MQTTPrefixEtat = GID("MQTTPrefixEtat").value.trim();
  F.MQTTdeviceName = GID("MQTTdeviceName").value.trim();
  F.TopicP= GID("TopicP").value.trim();
  F.nomRouteur = GID("nomRouteur").value.trim();
  F.nomSondeFixe = GID("nomSondeFixe").value.trim();
  F.nomSondeMobile = GID("nomSondeMobile").value.trim();
  F.CalibU = GID("CalibU").value;
  F.CalibI = GID("CalibI").value;
  F.CleAccesRef = GID("CleAcces").value.trim()

  for (var c = 0; c < 4; c++) {
    var QS = GID(`srcTemp${c}`).value;
    F["Source_Temp" +c] = (F.ModePara == 0) ? "tempNo" : QS;
    F["nomTemperature" +c] = GID(`nomTemperature${c}`).value.trim();
    F["TopicT"+c] = GID(`TopicT${c}`).value.trim();
    F["refTempIP"+c] = GID(`refTempIP${c}`).value;
    F["canalTempExterne"+c] = GID(`canalEXT${c}`).value;
    F["offsetTemp"+c] = GID(`offsetTemp${c}`).value * 100;
  }
  F.RMS_IP0 = ip2int(GID("adrIP").value);
  F.IP_Fixe = F.RMS_IP0;
  for (let i = 1; i < 8; i++) {
    let Vip = (F.ModePara == 0) ? 0 : ip2int(GID("RMS_IP" + i).value.trim());
    F["RMS_IP"+i] = Vip;
  }
  console.log(F);
  document.cookie = "CleAcces=" + encodeURIComponent(F.CleAccesRef) + ";max-age=31536000;path=/"; // Ajout de la durée et du chemin
  if ((GID("dhcp").checked || checkIP("adrIP") && checkIP("gateway")) && (!GID("MQTTRepet").checked || checkIP("MQTTIP"))) {
    fetch("/ParaNew", {
      method: "POST",                     // méthode HTTP
      headers: {
        "Content-Type": "application/json" // indique qu'on envoie du JSON
      },

      body: JSON.stringify(F)          // conversion de l'objet JS en texte JSON

    })
      .then(response => {
        if (!response.ok) throw new Error("Erreur HTTP " + response.status);
        return response.json();             // transforme la réponse JSON en objet JS
      })
      .then(resultat => {
        console.log("Réponse du serveur :", resultat);
        location.reload();
      })
      .catch(error => {
        console.error("Erreur d'envoi :", error);
      });
  }
}

)====";


const char * ParaJS2 = R"====(

/**
 * Envoie une requête au serveur pour réinitialiser les historiques.
 */
function SendRAZ() {
    if (confirm("Confirmez la remise à zéro des historiques")) {
        const xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState === 4 && this.status === 200) {
                // var retour = this.responseText; // Le retour n'est pas utilisé ici
            }
        };
        xhttp.open('GET', '/ajaxRAZhisto', true);
        xhttp.send();
    }
}

/**
 * Active ou désactive les champs de paramétrage en fonction du mode et de l'état du réseau.
 */
function checkDisabled() {
    // Rend le premier champ d'IP de routeur en lecture seule
    GID("RMS_IP0").style.backgroundColor = "#888";
    GID("RMS_IP0").readOnly = true;

    F.ModeReseau = GID("ModeW").value;
    const selectedMode = document.querySelector('input[name="ModeP"]:checked');
    F.ModePara = selectedMode ? selectedMode.value : '0'; 
 
    const pESP = GID("ESP").value;

    if (GID("sources").value =="Linky") GID("Serial2V").value=9600; //vitesse Linky en mode standard 
    GID("Vport_serie").style.display = (GID("Serie").value>0) ? "table-row": "none";

    // Mise à jour de l'affichage des boutons/lignes
    GID("Bwifi").style.display = (F.ESP32_Type < 10) ? "inline-block" : "none";
    
    // Affichage des paramètres IP statiques
    const ipSettingsVisible = !(GID("dhcp").checked || F.ModeReseau == 2);
    GID("infoIP").style.display = ipSettingsVisible ? "table" : "none";
    
    // Visibilité DHCP
    GID("dhcp").style.visibility = (F.ModeReseau == 2) ? "hidden" : "visible";
    GID("ipreset").style.display = (F.ModeReseau == 2) ? "none" : "inherit";
    
    const isESPTypeEcran = (pESP >= 4 && pESP<=9 );
    GID("rotation").style.display = isESPTypeEcran ? "table-row" : "none";
    GID("dureeOn").style.display = isESPTypeEcran ? "table-row" : "none";
    GID("Click35").style.display = isESPTypeEcran && GID("DurEcran").value>0  ? "table-row" : "none";
    GID("L_NumPageBoot").style.display = isESPTypeEcran  ? "table-row" : "none";

    
    
    // Lignes de WiFi (visibilité)
    GID("l_wifi_0").style.display = (F.ModeReseau == 2) ? "none" : "table-row";
    GID("l_wifi_1").style.display = (F.ModeReseau == 2 || F.ModePara == 0) ? "none" : "table-row";
    GID("l_wifi_2").style.display = (F.ModeReseau == 2 || F.ModePara == 0 || F.ESP32_Type >= 10) ? "none" : "table-row";

    // Visibilité de la liste des routeurs
    GID("listerouteurs").style.display = (F.ModeReseau == 2 || F.ModePara == 0) ? "none" : "block";
    
    // Désactivation des options de source si F.ModeReseau == 2 (AP mode) ou F.ModePara == 0 (Non Expert)
    const isDisabledAP = (F.ModeReseau == 2);
    const isDisabledExpert = (F.ModePara == 0 || F.ModeReseau == 2);

    for (let i = 5; i < 11; i++) {
        if (GID("sources").options[i]) {
            GID("sources").options[i].disabled = isDisabledAP;
        }
    }
    if (GID("sources").options[11]) {
        GID("sources").options[11].disabled = isDisabledExpert;
    }

    // Désactivation des options de température par canal
    for (let i = 0; i < 4; i++) {
        const srcTemp = GID('srcTemp' + i);
        if (srcTemp) {
            // Désactive tempExt si en mode AP
            const optExt = srcTemp.querySelector('option[value="tempExt"]');
            if (optExt) optExt.disabled = isDisabledAP;
            
            // Désactive tempMqtt si en mode AP
            const optMqtt = srcTemp.querySelector('option[value="tempMqtt"]');
            if (optMqtt) optMqtt.disabled = isDisabledAP;
        }
    }
    
    // Visibilité de l'envoi de données
    GID("donEnv").style.display = isDisabledAP ? "none" : "inline-block";
    
    // Mise à jour de l'IP affichée
    if (F.ModeReseau == 2) {
        GH("localIP", " : 192.168.4.1");
    } else {
        GH("localIP", "auto (DHCP) : " + V.localIP);
    }

    
    
    // Correction de l'horloge si le mode réseau n'est pas internet et Horloge est à 0
    if (F.ModeReseau ==2 && F.Horloge == 0) { 
        F.Horloge = 2; 
    }

    // Visibilité du bloc MQTT
    const isMqttVisible = (GID("MQTTRepet").value != 0 || GID("sources").value == "Pmqtt" || GID("subMQTT").checked) && F.ModePara > 0;
    GID("Zmqtt").style.display = isMqttVisible ? "block" : "none";
    
    // Visibilité des paramètres de température et de la surveillance
    const isExpertMode = (F.ModePara > 0);
    GID("LesSourcesTemp").style.display = isExpertMode ? "block" : "none";
    GID("pTemp").style.display = isExpertMode ? "table-row" : "none";
    GID("SurvCom").style.display = isExpertMode ? "block" : "none";
    
    // Gestion de l'affichage des lignes de paramètres de température
    for (let c = 0; c < 4; c++) {
        const srcValue = GID(`srcTemp${c}`).value;
        const isMqtt = srcValue === "tempMqtt";
        const isExt = srcValue === "tempExt";
        const isInt = srcValue === "tempInt";
        const isNo = srcValue === "tempNo";

        GID('ligneTemperature' + c).style.display = isNo ? "none" : "table";
        GID('ligneTopicT' + c).style.display = isMqtt ? "table-row" : "none";
        GID('ligneIPtemp' + c).style.display = isExt ? "table-row" : "none";
        GID('canalEXTtemp' + c).style.display = isExt ? "table-row" : "none";
        GID('ligneoffsetTemp' + c).style.display = isInt ? "table-row" : "none";
        
        // Si une source est MQTT, force l'affichage du bloc MQTT
        if (isMqtt) {
             GID("Zmqtt").style.display = "block"; // Utilisation de "block" pour écraser le "none" potentiel
        }
    }
    
    // Visibilité du Topic de Puissance (pour source 11/MQTT)
    GID('ligneTopicP').style.display = (GID("sources").value == "Pmqtt") ? "table-row" : "none";
    
    // Mise à jour et appel final
    F.Source = GID("sources").value;
    if (F.Source != 'Ext') V.Source_data = F.Source;
    AdaptationSource();
}

/**
 * Valide le format d'une adresse IP dans un champ donné.
 * @param {string} id - L'ID de l'élément input contenant l'IP.
 * @returns {boolean} Vrai si l'IP est valide, faux sinon.
 */
function checkIP(id) {
    const S = GID(id).value;
    const Table = S.split(".");
    let valide = true;
    
    if (Table.length !== 4) {
        valide = false;
    } else {
        for (const part of Table) {
            // Convertit en nombre et vérifie la plage
            const num = parseInt(part, 10);
            if (isNaN(num) || num > 255 || num < 0) {
                valide = false;
                break;
            }
        }
    }
    
    // Mise à jour du style pour feedback visuel
    GID(id).style.color = valide ? "black" : "red";
    
    return valide;
}

/**
 * Adapte l'affichage des champs de source de données en fonction de la source sélectionnée.
 */
function AdaptationSource() {
    // Visibilité des options de nom (Fixe)
    const isSourceDual = (V.Source_data === 'UxIx2' || ((V.Source_data === 'ShellyEm' || V.Source_data === 'ShellyPro') && GID("EnphaseSerial").value != 3));
    GID('ligneFixe').style.display = isSourceDual ? "table-row" : "none";
    GID('ligneFixe1').style.display = isSourceDual ? "table-row" : "none";
    GID('ligneFixe2').style.display = isSourceDual ? "table-row" : "none";
    
    // Visibilité de la calibration (seulement si UxI est la source et la source de données)
    const isSourceUxI = (V.Source_data === 'UxI' && F.Source === 'UxI');
    GID('Zcalib').style.display = isSourceUxI ? "table" : "none";
    GID('Analog').style.display = isSourceUxI ? "table-row" : "none";
    
    // Visibilité des options Linky/CACSI
    GID('CACSI').style.display = (F.Source === 'Linky') ? "table" : "none";
    
    // Détermination des libellés d'IP externe
    let txtExt = "ESP-RMS";
    let lab_enphaseShelly = "Numéro série passerelle IQ Enphase : <span class='fsize10'><br>Pour firmvare Envoy-S V7 seulement</span>";
    
    switch (F.Source) {
        case 'Enphase':
            txtExt = "Enphase-Envoy";
            break;
        case 'SmartG':
            txtExt = "SmartGateways";
            break;
        case 'HomeW':
            txtExt = "HomeWizard";
            break;
        case 'ShellyEm':
        case 'ShellyPro':
            txtExt = "Shelly (Pro) Em ";
            lab_enphaseShelly = `<div class='shem'><strong>Shelly (Pro) Em</strong><br>
                                 Monophasé : Courant maison sur voie 0,1 ou 2<br>
                                 Triphasé : mettre 3</div>
                                 <div class='shem'><Strong>Shelly Em Gen3</strong><br>
                                 Courant maison sur voie 0 = 30, voie 1 = 31</div>`;
            break;
    }
    
    // Mise à jour des libellés
    GH('labExtIp', txtExt);
    GH('label_enphase_shelly', lab_enphaseShelly);

    // Visibilité de la ligne d'IP externe/Référence
    const isExternalSource = ['Ext', 'Enphase', 'SmartG', 'HomeW', 'ShellyEm', 'ShellyPro'].includes(F.Source);
    GID('ligneExt').style.display = isExternalSource ? "table-row" : "none";

    // Visibilité des options d'authentification/série Enphase/Shelly
    GID('ligneEnphaseUser').style.display = (F.Source === 'Enphase') ? "table-row" : "none";
    GID('ligneEnphasePwd').style.display = (F.Source === 'Enphase') ? "table-row" : "none";
    GID('ligneEnphaseSerial').style.display = (F.Source === 'Enphase' || F.Source === 'ShellyEm' || F.Source === 'ShellyPro') ? "table-row" : "none";
}

/**
 * Termine le chargement des paramètres des routeurs et met à jour les sélecteurs de température externe.
 */
function SetParaVar() {
     GID("adrIP").value = int2ip(V.IP_Fixe); //Valeur actuelle et non enregistré en fichier F.IP_Fixe
    let Soptions = "";
    
    // Boucle pour remplir les IP/Noms des routeurs et construire les options de sélection
    for (let c = 0; c < nb_ESP; c++) {
        GID(`RMS_IP${c}`).value = V.IP_RMS[c];
        GH(`RMS_Nom${c}`, nomRMS[c]);
        GID(`Routeur_${c}`).style.display = "table-row"; 
        
        // Ajout des options pour les sélecteurs de température (exclut RMS_IP0, qui est l'IP locale)
        if (c > 0) {
            Soptions += `<option value=${c}>${V.IP_RMS[c]} ${nomRMS[c]}</option>`;
        }
        
        // Affichage de la ligne suivante (avec indication) pour permettre l'ajout d'une nouvelle IP
        const d = c + 1;
        if (c < 7) {
            GH("RMS_Nom" + d, "<small>Ajoutez une adresse IP de routeur ----></small>");
            // Correction: Utiliser style.display
            GID("Routeur_" + d).style.display = "table-row";
        }
    }

    // Mise à jour des sélecteurs de température externe
    for (let i = 0; i < 4; i++) {
        GH("refTempIP" + i, Soptions);
        GID("refTempIP" + i).value = refTempIP[i];
    }
    
    
    // Mettre à jour l'état des champs désactivés
    checkDisabled();
}
)====";

