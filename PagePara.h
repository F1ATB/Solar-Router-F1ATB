//***************************************************
// Page HTML  de gestion des Paramètres
//***************************************************
const char *ParaHtml = R"====(
<!doctype html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <link rel="stylesheet" href="commun.css">
  <script src="/CommunCouleurJS"></script>

  <style>
    .form { margin: auto; padding: 5px; display: table; text-align: left; width: 100%; }
    .form2 { margin: 2px; padding: 2px; display: block; width: 100%; }
    .ligne { display: table-row; }
    .ligne3 { display: flex; justify-content: center; padding: 0; margin: 0; }
    .cadre { border-top: 1px solid azure; }
    label, .nomR { display: table-cell; margin: 5px; text-align: left; font-size: 20px; height: 25px; width: 60%; }
    input, select { display: table-cell; margin: 5px; text-align: left; font-size: 20px; height: 25px; }
    select { display: table-cell; margin: 5px; text-align: left; }
    .source label { display: table-cell; margin: 5px; text-align:left; font-size:12px; height:21px; width:initial; }
    .boldT { text-align:left; font-weight:bold; padding:10px; }
    .Bparametres { border: inset 10px azure; }
    .Bgeneraux { border: inset 4px azure; }
    #BoutonsBas { text-align:center; }
    #ligneFixe, .ligneTemperature, #ligneExt, #ligneEnphaseUser, #ligneEnphasePwd, #ligneEnphaseSerial,
    #infoIP, #ligneTopicP, #ligneTopicT { display:none; }
    .Zone, .generaux { width:100%; border:1px solid grey; border-radius:10px; margin-top:10px;
                       background-color:rgba(30,30,30,0.3); }
    #onglets2 { display:block; }
    .don { height:50px; display:block; text-align:center; }
    #donEnv { display:inline-block; text-align:center; border:azure 3px outset;
              border-radius:18px; background-color:white; }
    .donNone { display:none; }
    .generaux { width:60%; text-align:center; margin:auto; }
    .generaux label { width:30%; }
    .fsize150 { font-size:140%; }
    .topG { display:flex; justify-content:center; }
    .shem { font-size:12px; display:flex; }
    .tc { text-align:center; background:rgba(50,50,60,0.2); padding:4px; }
    #LesParas{display:none;}
  </style>

  <title>Params F1ATB</title>
</head>

<body onload="Init();">

  <div id="lesOnglets"></div>
  <div id="LesParas">
    <div class="generaux">
      <div class="fsize150">Paramètres généraux</div>
      <div class="topG">
        <label for="ModeP0" style="text-align:right;">Mode standard</label>
        <input type="radio" name="ModeP" id="ModeP0" value="0" checked onclick="checkDisabled();">
        <label for="ModeP1" style="text-align:right;">Mode expert</label>
        <input type="radio" name="ModeP" id="ModeP1" value="1" onclick="checkDisabled();">
      </div>
    </div>

    <div class="Zone">
      <div class="boldT">Routeur</div>
      <div class="form">

        <div class="ligne">
          <label for="nomRouteur">Nom du routeur :</label>
          <input type="text" id="nomRouteur" name="nomRouteur">
        </div>

        <div class="ligne" id="ligneMobile">
          <label for="nomSondeMobile">Nom données puissance entrée de Maison</label>
          <input type="text" id="nomSondeMobile" name="nomSondeMobile">
        </div>

        <div class="ligne" id="ligneFixe">
          <label for="nomSondeFixe">Nom données courant seconde sonde</label>
          <input type="text" id="nomSondeFixe" name="nomSondeFixe"
                title="Champ obligatoire si vous avez 2 sondes">
        </div>
        <div class="ligne" id="ligneFixe1">
          <label for="nomSfixePpos">Nom si Pw seconde sonde >0 <small>(Pas de nom = suppression colone)</small></label>
          <input type="text" id="nomSfixePpos" name="nomSfixePpos"
                title="Champ obligatoire si puissance positive">
        </div>
        <div class="ligne" id="ligneFixe2">
          <label for="nomSfixePneg">Nom si Pw seconde sonde <0 <small>(Pas de nom = suppression colone)</small></label>
          <input type="text" id="nomSfixePneg" name="nomSfixePneg"
                title="Champ obligatoire si puissance négative">
        </div>

        <div class="ligne" id="l_wifi_0">
          <label for="TempoRTEon">Affichage couleur Tempo RTE </label>
          <input type="checkbox" id="TempoRTEon" name="TempoRTEon" style="width:25px;"
                title="Si vous avez un tarif Tempo.">
          <span class="fsize10">Nécessite un Reset de l'ESP32</span>
        </div>

        <div class="ligne" id="l_wifi_1">
          <label for="MQTTRepet">Puissances et températures envoyées au serveur MQTT<br>
            Période (s) répétition (0 = pas d'envoi) </label>
          <input type="number" id="MQTTRepet" name="MQTTRepet"
                title="Mettre 10 ou plus pour ne pas surcharger les communications."
                onclick="checkDisabled();">
        </div>

        <div class="ligne" id="l_wifi_2">
          <label for="WifiSleep">
            Wifi Sleep/veille <small>(réduit la consommation mais ralentit la communication)</small> 
          </label>
          <input type="checkbox" id="WifiSleep" name="WifiSleep" style="width:25px;">
          <span class="fsize10">Nécessite un Reset de l'ESP32</span>
        </div>

        <div class="ligne">
          <label for="RAZdata">Remise à zéro historique des mesures</label>
          <input class="bouton" type="button" id="RAZdata" onclick="SendRAZ();" value="RAZ"
                title="Attention vous allez perdre les historiques des mesures enregistrées.">
        </div>

      </div>

      <div>Configuration matérielle <span class="fsize10">Nécessite un Reset de l'ESP32</span></div>

      <div class="form">

        <div class="ligne">
          <label for="ESP">Modèle carte ESP32</label>
          <select id="ESP" onclick="checkDisabled();"
                  title="Carte à définir pour connaître les GPIOs disponibles.">
            <option value="0" selected>Non défini</option>
            <option value="1">Wroom seul</option>
            <option value="2">Carte 1 relais</option>
            <option value="3">Carte 4 relais</option>
            <option value="4">Ecran ESP32-2432S028R R_ILI9341</option>
            <option value="5">Ecran ESP32-2432S028 R_ST7789</option>
            <option value="6">Ecran ESP32-2432S024 R_ILI9341</option>
            <option value="7">Ecran ESP32-024 R_ST7789</option>
            <option value="8">Ecran ESP32-2432S024C C_ILI9341</option>
            <option value="9">Ecran JC2432W328 C_ST7789</option>
            <option value="10">WT32-ETH01</option>
          </select>
        </div>

        <div class="ligne" id="rotation">
          <label for="Rot">Format écran</label>
          <select id="Rot" onclick="checkDisabled();">
            <option value="0" selected>Portrait (0°)</option>
            <option value="1">Paysage (90°)</option>
            <option value="2">Portrait (180°)</option>
            <option value="3">Paysage (270°)</option>
          </select>
        </div>

        <div class="ligne" id="dureeOn">
          <label for="DurEcran">Durée écran On</label>
          <select id="DurEcran" onclick="checkDisabled();">
            <option value="10000" selected>10s</option>
            <option value="30000">30s</option>
            <option value="300000">5mn</option>
            <option value="54000000">15h</option>
            <option value="0">Permanent</option>
          </select>
        </div>
        <div class="ligne" id="Click35">
          <label for="clickPresence">Allumage écran</label>
          <select id="clickPresence" >
            <option value="0" selected>Click</option>
            <option value="1">Click ou présence GPIO35</option>
          </select>
        </div>
        <div class="ligne" id="L_NumPageBoot">
          <label for="NumPageBoot">Page d'accueil écran</label>
          <select id="NumPageBoot" >
            <option value="0" selected>Tableau mesures</option>
            <option value="6">Gauge Puissance</option>
            <option value="5">Couleur fond écran</option>
          </select>
        </div>

        <div class="ligne" id="port_serie">
          <label for="Serie">Port série 2 <span class="fsize10">Pour UxIx2, UxIx3 ou Linky</span></label>
          <select id="Serie" onclick="checkDisabled();">
            <option value="0" selected>Non utilisé</option>
            <option value="1">RX=gpio 16, TX=gpio 17</option>
            <option value="2">RX=gpio 26, TX=gpio 27</option>
            <option value="3">RX=gpio 18, TX=gpio 19</option>
            <option value="4">RX=gpio 5, TX=gpio 17</option>
            <option value="5">RX=gpio 21, TX=gpio 22</option>
          </select>
        </div>

        <div class="ligne" id="Vport_serie">
          <label for="Serial2V">Vitesse Port série 2 <span class="fsize10">Défaut : UxIx2=4800, UxIx3=9600, Linky=9600</span></label>
          <select id="Serial2V" onclick="checkDisabled();">
            <option value="0" selected>Pas utilisé</option>
            <option value="4800">4800 bauds</option>
            <option value="9600">9600 bauds</option>
            <option value="19200">19200 bauds</option>
            <option value="38400">38400 bauds</option>
          </select>
        </div>

        <div class="ligne" id="Analog">
          <label for="pUxI">GPIOs Entrées analogiques</label>
          <select id="pUxI" onclick="checkDisabled();">
            <option value="0" selected>Pas d'entrée</option>
            <option value="1">Commun(35), U(32), I(33)</option>
            <option value="2">Commun(35), U(32), I(34)</option>
            <option value="3">Commun(34), U(32), I(33)</option>
            <option value="4">Commun(35), U(36), I(39)</option>
          </select>
        </div>

        <div class="ligne">
          <label for="pTriac">GPIOs Triac</label>
          <select id="pTriac" onclick="checkDisabled();">
            <option value="0" selected>Pas de triac</option>
            <option value="1">Pulse=gpio 4, Zc=gpio 5</option>
            <option value="2">Pulse=gpio 22, Zc=gpio 23</option>
            <option value="3">Pulse=gpio 21, Zc=gpio 22</option>
            <option value="4">Pulse=gpio 12, Zc=gpio 14</option>
          </select>
        </div>

        <div class="ligne">
          <label for="pLED">Affichage LED / OLED</label>
          <select id="pLED" onclick="checkDisabled();">
            <option value="0" selected>Pas d'affichage</option>
            <option value="1">LEDs gpio 18, gpio 19</option>
            <option value="2">LEDs gpio 4, gpio 16</option>
            <option value="3">LEDs gpio 2, gpio 4</option>
            <option value="4">LEDs gpio 4, gpio 17</option>
            <option value="10">SSD1306/1309 sda=g18, scl=g19</option>
            <option value="11">SSD1306/1309 sda=g4, scl=g32</option>
            <option value="12">SH1106 sda=g18, scl=g19</option>
            <option value="13">SH1106 sda=g4, scl=g32</option>
          </select>
        </div>

        <div class="ligne" id="pTemp">
          <label for="PTemp">Capteur(s) de température</label>
          <select id="PTemp" onclick="checkDisabled();">
            <option value="0" selected>Pas de capteur</option>
            <option value="1">DS18B20 gpio 13</option>
            <option value="4">DS18B20 gpio 21</option>
            <option value="2">DS18B20 gpio 27</option>
            <option value="3">DS18B20 gpio 33</option>
          </select>
        </div>

      </div>
    </div>

    <div class="Zone">
      <div class="boldT">Réseau et adresse IP</div>

      <div class="form">

        <div class="ligne">
          <label for="ModeW">Accès</label>
          <select id="ModeW" onclick="checkDisabled();">
            <option value="0" selected>Internet</option>
            <option value="1">Réseau local uniquement</option>
            <option value="2">Point isolé</option>
          </select>
        </div>
        <div class="ligne">
          <label for="hostname">Hostname (adresse web : hostname.local)</label>
          <input type="text" id="hostname" name="hostname"  title="Caractères a-z, - , 0-9. Permet l'accès à l'adresse 'hostname.local'.">
        </div>

        <div class="ligne">
          <label for="dhcp">Adresse IP <span id="localIP"></span></label>
          <input type="checkbox" id="dhcp" name="dhcp" style="width:25px;" onclick="checkDisabled();">
          <span id="ipreset" class="fsize10">Nécessite un Reset de l'ESP32</span>
        </div>

      </div>

      <div class="form" id="infoIP">

        <div class="ligne">
          <label for="adrIP">Adresse IP si fixe (ex : 192.168.1.245)
            <br><span class="fsize10">Nécessite un Reset de l'ESP32</span>
          </label>
          <input type="text" id="adrIP" name="adrIP"  minlength=7 maxlength=15 placeholder="xxx.xxx.xxx.xxx" title="4 groupes de nombres entre 0 et 255 separ&eacute;s par ." required>
        </div>

        <div class="ligne">
          <label for="gateway">Passerelle / Gateway (ex : 192.168.1.254)</label>
          <input type="text" id="gateway" name="gateway"   minlength=7 maxlength=15 placeholder="xxx.xxx.xxx.xxx" title="En général, adresse interne de la box internet">
        </div>

        <div class="ligne">
          <label for="masque">Masque / Subnet (ex : 255.255.255.0)</label>
          <input type="text" id="masque" name="masque" minlength=7 maxlength=15 placeholder="xxx.xxx.xxx.xxx" title="4 groupes de nombres entre 0 et 255 separ&eacute;s par ." >
        </div>

        <div class="ligne">
          <label for="dns">DNS (ex : 192.168.1.254)</label>
          <input type="text" id="dns" name="dns" minlength=7 maxlength=15 placeholder="xxx.xxx.xxx.xxx" title="En général, adresse interne de la box internet">
        </div>
      </div>
      
    </div>

    <div class="Zone" id="SurvCom">
      <div class="boldT">Surveillance communications</div>
      <div class="form">

        <div class="ligne">
          <label for="ComSurv">Reset si coupure WiFi ou Ethernet de</label>
          <select id="ComSurv" onclick="checkDisabled();">
            <option value="6" selected>3mn</option>
            <option value="60">30mn</option>
            <option value="360">3h</option>
            <option value="1440">12h</option>
            <option value="2880">24h</option>
          </select>
        </div>

      </div>
    </div>

    <div class="Zone" id="listerouteurs">
      <div class="boldT">
        Liste des Routeurs en réseau <span class="fsize10">Même version de routeur recommandée</span>
      </div>
      <div class="form" id="Routeurs"></div>
    </div>

    <div class="Zone">
      <div class="boldT">Mesures de puissance</div>
      <div class="form">

        <div class="ligne">
          <label for="sources">Source</label>
          <select id="sources" onclick="checkDisabled();"
                  title="Suivant l'interface de mesure choisie.">
            <option value="NotDef" selected>Non définie</option>
            <option value="UxI">UxI</option>
            <option value="UxIx2">UxIx2</option>
            <option value="UxIx3">UxIx3</option>
            <option value="Linky">Linky</option>
            <option value="Enphase">Enphase-Envoy</option>
            <option value="SmartG">SmartGateways</option>
            <option value="HomeW">HomeWizard</option>
            <option value="ShellyEm">Shelly Em</option>
            <option value="ShellyPro">Shelly Pro Em</option>
            <option value="Ext">ESP Externe</option>
            <option value="Pmqtt">MQTT</option>
          </select>
        </div>

      </div>

      <div class="form">

        <div class="ligne" id="ligneExt">
          <label for="RMSextIP">Adresse IP <span id="labExtIp"></span> externe :</label>
          <input type="text" id="RMSextIP" name="RMSextIP" autocomplete="on">
        </div>

        <div class="ligne" id="ligneEnphaseUser">
          <label for="EnphaseUser">Enphase Envoy-S metered User :
            <span class="fsize10"><br>Pour firmware Envoy-S V7 seulement</span>
          </label>
          <input type="text" id="EnphaseUser" name="EnphaseUser" autocomplete="on">
        </div>

        <div class="ligne" id="ligneEnphasePwd">
          <label for="EnphasePwd">Enphase Envoy-S metered Password :
            <span class="fsize10"><br>Pour firmware Envoy-S V7 seulement</span>
          </label>
          <input type="password" id="EnphasePwd" name="EnphasePwd" autocomplete="on">
        </div>

        <div class="ligne" id="ligneEnphaseSerial">
          <label for="EnphaseSerial" id="label_enphase_shelly"></label>
          <input type="text" id="EnphaseSerial" name="EnphaseSerial"
                onchange="checkDisabled();" autocomplete="on">
        </div>

        <div class="ligne" id="ligneTopicP">
          <label for="TopicP">MQTT Topic Puissance :</label>
          <input type="text" id="TopicP" name="TopicP" autocomplete="on">
        </div>

        <div><span class="fsize10">Nécessite un Reset de l'ESP32</span></div>

      </div>

      <div id="CACSI" class="form">
        <div class="ligne">
          <label for="EstimCACSI">Estimateur injection si CACSI :</label>
          <input type="checkbox" id="EstimCACSI" name="EstimCACSI" style="width:25px;"
                onclick="checkDisabled();"
                title="Estimation des injections basée sur les V*A données par le LINKY">
        </div>
      </div>

    </div>

    <div class="Zone" id="LesSourcesTemp">
      <div class="boldT">Source des mesures de température</div>
      <div class="form">
        <div id="Sources_Temp"></div>
      </div>
    </div>

    <div class="Zone" id="Zmqtt">
      <div class="boldT">Paramètres serveur MQTT <small>(Home Assistant, Domoticz ...)</small></div>

      <div class="form">

        <div class="ligne">
          <label for="MQTTIP">Adresse IP host MQTT :</label>
          <input type="text" id="MQTTIP" name="MQTTIP">
        </div>

        <div class="ligne">
          <label for="MQTTPort">Port :</label>
          <input type="number" id="MQTTPort" name="MQTTPort">
        </div>

        <div class="ligne">
          <label for="MQTTUser">MQTT User nom :</label>
          <input type="text" id="MQTTUser" name="MQTTUser" autocomplete="on">
        </div>

        <div class="ligne">
          <label for="MQTTPwd">MQTT mot de passe :</label>
          <input type="password" id="MQTTPwd" name="MQTTPwd" autocomplete="on">
        </div>

        <div class="ligne">
          <label for="MQTTPrefix">MQTT Préfixe de découverte :</label>
          <input type="text" id="MQTTPrefix" name="MQTTPrefix">
        </div>

        <div class="ligne">
          <label for="MQTTPrefixEtat">MQTT Préfixe de publication :</label>
          <input type="text" id="MQTTPrefixEtat" name="MQTTPrefixEtat">
        </div>

        <div class="ligne">
          <label for="MQTTdeviceName">MQTT Device Name / Nom routeur :</label>
          <input type="text" id="MQTTdeviceName" name="MQTTdeviceName">
        </div>

        <div class="ligne">
          <label for="subMQTT">Souscrire au forçage des Actions via MQTT :</label>
          <input type="checkbox" id="subMQTT" name="subMQTT" style="width:25px;" onclick="checkDisabled();">
        </div>

      </div>
    </div>

    <div class="Zone" id="Zcalib">
      <div class="boldT" id="Tui">
        Calibration Mesures Ueff et Ieff <small>(UxI)</small>
      </div>

      <div class="form">

        <div class="ligne" id="CUi">
          <label for="CalibU">Coefficient multiplicateur Ueff (typique : 1000) :</label>
          <input type="number" id="CalibU" name="CalibU">
        </div>

        <div class="ligne" id="CuI">
          <label for="CalibI">Coefficient multiplicateur Ieff (typique : 1000) :</label>
          <input type="number" id="CalibI" name="CalibI">
        </div>

      </div>
    </div>

    <div class="Zone">
      <div class="boldT">Sécurité d'accès aux paramètres et Actions</div>
      <div class="form">
        <div class="ligne">
          <label for="CleAcces">Définissez un mot de passe d'accès
            <span class="fsize10">Pas de ";" — Ne rien mettre si accès libre</span> :
          </label>
          <input type="text" id="CleAcces" name="CleAcces">
        </div>
      </div>
    </div>

    <div id="BoutonsBas">
      <br>
      <input class="bouton" type="button" onclick="SendValues();" value="Sauvegarder">
      <div class="lds-dual-ring" id="attente"></div>
      <input class="bouton" type="button" onclick="Reset();" value="ESP32 Reset">
    </div>
  </div>
  <br>
  <div id="pied"></div>
  <br>

  <div id="donEnv">
    <form action="https://www.paypal.com/donate" method="post" target="_top">
      <input type="hidden" name="hosted_button_id" value="Z35E9D5D9N9DN">
      <input class="don" type="image"
             src="https://pics.paypal.com/00/s/MGY1NzdhY2YtYTRkNi00YzIwLWI2YzQtNWI3YjM3ZmFiNWUx/file.PNG"
             style="border:0;" name="submit"
             title="PayPal - The safer, easier way to pay online!"
             alt="Bouton Faites un don avec PayPal">
      <img alt="" class="donNone" style="border:0;"
           src="https://www.paypal.com/fr_FR/i/scr/pixel.gif" width="1" height="1">
    </form>
  </div>

  <script src="/ParaCommunJS"></script>
  <script src="/ParaJS1"></script>
  <script src="/ParaJS2"></script>

</body>
</html>

)====";