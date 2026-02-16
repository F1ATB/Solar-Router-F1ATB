//******************************************
// Page données RMS Brutes HTML
//******************************************
const char *PageBrute = R"====(
<!doctype html>
<html lang="fr">

<head>
  <meta charset="UTF-8">
  <title>Data brute F1ATB</title>

  <link rel="stylesheet" href="/commun.css">
  <script src="/CommunCouleurJS"></script>

  <style>
    .ri { text-align: right; }
    .Wh { background-color: #fdd; }
    .A { background-color: #ddf; }
    .W { background-color: #f88; }
    .phi { background-color: #ffd; }
    .V { background-color: #ee8; }
    .VA { background-color: #dfd; }
    .Hz, .Enph { background-color: #eeb; }

    .titre {
      background-color: #ccc;
      text-align: center;
      font-weight: bold;
    }

    .dataIn {
      text-align: left;
      overflow: hidden;
      word-wrap: break-word;
    }

    td {
      text-align: left;
      padding: 4px;
    }

    #LED {
      position: absolute;
      top: 4px;
      left: 4px;
      width: 0;
      height: 0;
      border: 5px solid red;
      border-radius: 5px;
    }

    .Bbrut { border: inset 8px azure; }

    .dispT { display: none; }

    .ce {
      text-align: center;
      position: relative;
    }

    svg { border: 10px inset azure; }

    /* Sections masquées par défaut */
    #infoUxIx2,
    #infoUxIx3,
    #infoUxI,
    #infoNotDef,
    #infoLinky,
    #infoEnphase,
    #infoSmartG,
    #infoHomeW,
    #infoShellyEm,
    #infoWesV2,
    #infoPmqtt {
      display: none;
    }
    #DataLinky{
      font-size:14px;
      tab-size:10;
    }
    #donneeDistante {
      font-size: 50%;
      text-align: center;
      margin-bottom: 10px;
      display: none;
    }

    .bloc a:link,
    .bloc a:visited {
      color: #116;
      text-decoration: none;
    }
  </style>
</head>

<body onload="SetHautBas(); LoadParaFixe();">

  <div id="LED"></div>

  <div id="lesOnglets"></div>

  <div id="date">Date</div>
  <br><br>

  <!-- Information source non définie -->
  <div id="infoNotDef">
    <div class="tableau">
      Source des mesures de puissance non définie<br>
      A définir au bas de la page Paramètres<br><br>
      Données simulées
    </div>
    <br><br>
  </div>

  <!-- U/I 20ms -->
  <div id="infoUxI">
    <div>Tension et Courant sur 20ms</div>
    <div class="ce">
      <h3 style="position:absolute; top:20px; right:40px;">
        <span id="Ueff">.</span>
        <span id="Ieff"></span><br>
        <span id="cosphi"></span>
      </h3>
      <p id="SVG"></p>
    </div>
  </div>

  <!-- JSY-MK-194T -->
  <div id="infoUxIx2">
    <br><br>
    <div>Données brutes capteur JSY-MK-194T</div>
    <div id="tableau" class="tableau"></div>
  </div>

  <!-- JSY-MK-333 -->
  <div id="infoUxIx3">
    <div>Données brutes capteur JSY-MK-333</div>
    <div id="dataUxIx3" class="tableau dataIn"></div>
  </div>

  <!-- Enphase -->
  <div id="infoEnphase">
    <br><br>
    <div>Données Enphase Envoy-S Metered</div>
    <div id="tableauEnphase" class="tableau"></div>
  </div>

  <!-- SmartGateways -->
  <div id="infoSmartG">
    <div>Données SmartGateways</div>
    <div id="dataSmartG" class="tableau dataIn"></div>
  </div>

  <!-- HomeWizard -->
  <div id="infoHomeW">
    <div>Données HomeWizard</div>
    <div id="dataHomeW" class="tableau dataIn"></div>
  </div>

  <!-- Shelly EM -->
  <div id="infoShellyEm">
    <div>Données Shelly Em</div>
    <div id="dataShellyEm" class="tableau dataIn"></div>
  </div>

  <!-- MQTT -->
  <div id="infoPmqtt">
    <div>Données puissances reçues par MQTT</div>
    <div id="dataPmqtt" class="tableau dataIn"></div>
  </div>

  <!-- WES v2 -->
  <div id="infoWesV2">
    <div>Données WES v2 Cartelectronic</div>
    <div id="dataWesV2" class="tableau dataIn"></div>
  </div>

  <!-- Linky -->
  <div id="infoLinky">
    <div id="dateLinky"></div>
    <div id="tableauLinky" class="tableau"></div>
    <br><br>
    <div>Données brutes Linky en mode standard</div>
    <div id="DataLinky" class="tableau dataIn"></div>
  </div>

  <div id="donneeDistante">Données distantes</div>

  <!-- Données ESP32 -->
  <div>Données ESP32</div>
  <div id="DataESP32" class="tableau bloc"></div>
  <br>

  <div id="pied"></div>

  <!-- Scripts -->
  <script src="/ParaCommunJS"></script>
  <script src="BruteJS1"></script>
  <script src="BruteJS2"></script>

</body>
</html>
)====";