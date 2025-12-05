const char *ParaCleHtml = R"====(
<!doctype html>
<html>

<head>
  <meta charset="UTF-8">
  <link rel="stylesheet" href="/commun.css">
  <style>
    body {
      color: white;
    }

    .form {
      margin: auto;
      padding: 10px;
      display: table;
      text-align: left;
      width: 100%;
    }

    .ligne {
      display: table-row;
      padding: 10px;
    }

    .cadre {
      border-top: 1px solid azure;
    }

    label,
    .nomR {
      display: table-cell;
      margin: 5px;
      text-align: left;
      font-size: 20px;
      height: 25px;
      width: 60%;
    }

    input {
      display: table-cell;
      margin: 5px;
      text-align: left;
      font-size: 20px;
      height: 25px;
    }

    .boldT {
      text-align: left;
      font-weight: bold;
      padding: 10px;
    }

    .Bparametres {
      border: inset 10px azure;
    }

    .Bgeneraux {
      border: inset 4px azure;
    }

    #BoutonsBas {
      text-align: center;
    }

    .Zone {
      width: 100%;
      border: 1px solid grey;
      border-radius: 10px;
      margin-top: 10px;
      background-color: rgba(30, 30, 30, 0.3);
    }

    #onglets2 {
      display: none;
    }
  </style>
  <script>
    var BordsInverse = [".Bparametres"];
    function SendCle() {
      document.cookie = "CleAcces=" + GID("CleAcces").value.trim();

      GID("attente").style = "visibility: visible;";
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
          var retour = this.responseText;
          location.reload();
        }
      };
      xhttp.open('GET', '/CleUpdate', true);
      xhttp.send();
    }
    function init() {
      SetHautBas();
      LoadParaRouteur();
      LoadCouleurs();
      document.cookie = "CleAcces=" + GID("CleAcces").value.trim();
    }
    function AdaptationSource() { };
    function FinParaRouteur() { };
  </script>
  <title>Passe Accès Routeur</title>
</head>

<body onload="init();">
  <div id="lesOnglets"></div>
  <h4>Mot de passe d'accès</h4>
  <div class="Zone">
    <div class="boldT">Sécurité d'accès aux paramètres et Actions</div>
    <div class="form">
      <div class="ligne">
        <label for="CleAcces">Entrez le mot de passe d'accès : </label>
        <input type="text" name="CleAcces" id="CleAcces">
      </div>
    </div>
  </div>
  <div id="BoutonsBas">
    <br><input class="bouton" type="button" onclick="SendCle();" value="Envoyer">
    <div class="lds-dual-ring" id="attente"></div>
  </div>
  <br>
  <div id="pied"></div>
  <script src="/ParaRouteurJS"></script>
  <script src="/CommunCouleurJS"></script>
</body>

</html>
)====";

const char *CommunCSS = R"====(
a:link {color:#aaf;text-decoration: none;}
a:visited {color:#ccf;text-decoration: none;}
.onglets{margin-top:4px;left:0px;font-size:130%;}
#onglets2{margin-top:10px;left:0px;font-size:80%;display:none;}
.Bonglet{margin-left:20px;border:outset 4px
grey;background-color:#333;border-radius:6px;padding-left:20px;padding-right:20px;display:inline-block;}
.Bonglet2{margin-left:20px;border:outset 2px
grey;background-color:#333;border-radius:4px;padding-left:20px;padding-right:20px;display:inline-block;}
#pied{display:flex;justify-content:space-between;font-size:14px;}
.fsize12{font-size:12px;height:16px;}
.fsize10{font-size:10px;height:14px;}
.fsize8{font-size:8px;}
.tableau { background-color:white;display:inline-block;margin:auto;padding:4px;color:black;border:2px inset
grey;border-radius:10px;}
table{border-collapse:collapse;}
.lds-dual-ring {color: #cccc5b;visibility: hidden;}
.lds-dual-ring,.lds-dual-ring:after {box-sizing: border-box;}
.lds-dual-ring {display: inline-block;width: 80px;height: 80px;}
.lds-dual-ring:after {content: " ";display: block;width: 64px;height: 64px;margin: 8px;border-radius: 50%;border: 6.4px
solid currentColor;border-color: currentColor transparent currentColor transparent;animation: lds-dual-ring 1.2s linear
infinite;}
@keyframes lds-dual-ring {0% {transform: rotate(0deg);} 100% {transform: rotate(360deg);}}
.bouton,input[type=file]::file-selector-button{margin: 5px;text-align:left;font-size:20px;height:28px;border:3px grey
outset;border-radius:7px;cursor:pointer;}
)====";