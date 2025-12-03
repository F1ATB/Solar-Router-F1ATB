//******************************************
// Page données RMS Brutes HTML
//******************************************
const char *PageBrute = R"====(
<!doctype html>
<html>

<head>
  <meta charset="UTF-8">
  <link rel="stylesheet" href="/commun.css">
   <script src="/CommunCouleurJS"></script>
  <style>
    .ri {
      text-align: right;
    }

    .Wh {
      background-color: #fdd;
    }

    .A {
      background-color: #ddf;
    }

    .W {
      background-color: #f88;
    }

    .phi {
      background-color: #ffd;
    }

    .V {
      background-color: #ee8;
    }

    .VA {
      background-color: #dfd;
    }

    .Hz,
    .Enph {
      background-color: #eeb;
    }

    .titre {
      background-color: #ccc;
      text-align: center;
      font-weight: bold;
    }

    .dataIn {
      text-align: left;
      overflow: hidden;
    }

    td {
      text-align: left;
      padding: 4px;
    }

    #LED {
      position: absolute;
      top: 4px;
      left: 4px;
      width: 0px;
      height: 0px;
      border: 5px solid red;
      border-radius: 5px;
    }

    .Bbrut {
      border: inset 8px azure;
    }

    .dispT {
      display: none;
    }

    .ce {
      text-align: center;
      position: relative;
    }

    svg {
      border: 10px inset azure;
    }

    #infoUxIx2,
    #infoUxIx3,
    #infoUxI,
    #infoNotDef,
    #infoLinky,
    #infoEnphase,
    #infoSmartG,
    #infoHomeW,
    #infoShellyEm,
    #infoPmqtt {
      display: none;
    }

    #donneeDistante {
      font-size: 50%;
      text-align: center;
      margin-bottom: 10px;
      display: none;
    }

    .bloc a:link {
      color: #116;
      text-decoration: none;
    }

    .bloc a:visited {
      color: #226;
      text-decoration: none;
    }
  </style>
  <title>Data brute F1ATB</title>
</head>

<body onload="SetHautBas();LoadParaRouteur();">
  <div id="LED"></div>
  <div id="lesOnglets"></div>
  <div id="date">Date</div><br><br>
  <div id="infoNotDef">
    <div class="tableau">Source des mesures de puissance non définie<br>A définir au bas de la page
      Paramètres<br><br>Données simulées</div> <br><br>
  </div>
  <div id="infoUxI">
    <div>Tension et Courant sur 20ms</div>
    <div class="ce">
      <h3 style="position:absolute;top:20px;right:40px;"><span id="Ueff">.</span>
        <span id="Ieff"></span><br>
        <span id="cosphi"></span>
      </h3>
      <p id="SVG"></p>
    </div>
  </div>
  <div id="infoUxIx2">
    <br><br>
    <div>Donn&eacute;es brutes capteur JSY-MK-194T</div>
    <div id="tableau" class="tableau"></div>
  </div>
  <div id="infoUxIx3">
    <div>Donn&eacute;es brutes capteur JSY-MK-333</div>
    <div id="dataUxIx3" class="tableau dataIn"></div>
  </div>
  <div id="infoEnphase">
    <br><br>
    <div>Donn&eacute;es Enphase Envoy-S Metered</div>
    <div id="tableauEnphase" class="tableau"></div>
  </div>
  <div id="infoSmartG">
    <div>Donn&eacute;es SmartGateways</div>
    <div id="dataSmartG" class="tableau dataIn"></div>
  </div>
  <div id="infoHomeW">
    <div>Donn&eacute;es HomeWizard</div>
    <div id="dataHomeW" class="tableau dataIn"></div>
  </div>
  <div id="infoShellyEm">
    <div>Donn&eacute;es Shelly Em </div>
    <div id="dataShellyEm" class="tableau dataIn"></div>
  </div>
  <div id="infoPmqtt">
    <div>Données puissances recues par MQTT</div>
    <div id="dataPmqtt" class="tableau dataIn"></div>
  </div>
  <div id="infoLinky">
    <div id="dateLinky"></div>
    <div id="tableauLinky" class="tableau"></div>
    <br><br>
    <div>Donn&eacute;es brutes Linky en mode standard</div>
    <div>
      <div id="DataLinky" class="tableau dataIn"></div>
    </div>
  </div>
  <div id="donneeDistante">Données distantes</div>
  <div>Donn&eacute;es ESP32</div>
  <div>
    <div id="DataESP32" class="tableau bloc"></div>
  </div><br>
  <div id="pied"></div>
  <script src="/ParaRouteurJS"></script>
  <script src="BruteJS1"></script>
  <script src="BruteJS2"></script>
 
  <br>
</body>

</html>
)====";