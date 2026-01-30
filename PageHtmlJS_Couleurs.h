//****************************************************
// Page HTML et Javascript - Gestion des couleurs    *
//****************************************************
const char *CouleursHtml = R"====(
 <!doctype html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Colors</title>

    <link rel="stylesheet" href="/commun.css">

    <style>
        .Zone {
            width: 100%;
            border: 1px solid grey;
            border-radius: 10px;
            margin-top: 10px;
            background-color: rgba(30,30,30,0.3);
        }
        .boldT {
            text-align: left;
            font-weight: bold;
            padding: 10px;
        }
        .form {
            margin: auto;
            padding: 10px;
            display: table;
            text-align:left;
            width:100%;
        }
        .ligne {
            display: table-row;
            margin-top: 5px;
        }
        .ligneB {
            font-weight: bold;
        }
        .ligne div {
            display: table-cell;
            margin: 5px;
            text-align: left;
            font-size: 20px;
            height: 25px;
        }
        .liste {
            display:flex;
            justify-content:center;
            text-align:left;
        }
        #onglets2 {
            display:block;
        }
        .Bparametres { border: inset 10px azure; }
        .Bcouleurs   { border: inset 4px azure; }
        .les_boutons {
            display:flex;
            justify-content:space-between;
        }
    </style>
</head>

<body onload="Init();">

    <div id="lesOnglets"></div>

    <h2>Choix des couleurs</h2>

    <div class="Zone">
        <div class="form" id="colors"></div>
    </div>

    <br>

    <div class="les_boutons">
        <input class="bouton" type="button" value="Couleurs par défaut"
               onclick="SendValues(false);">

        <input class="bouton" type="button" value="Sauvegarder"
               onclick="SendValues(true);">
    </div>
   <br>
    <script>
        var BordsInverse = [".Bparametres", ".Bcouleurs"];

        function Init() {
            SetHautBas();
            SetCurseurs();
            LoadParaFixe();
            
        }

        function SetCurseurs() {
            let S = "<div class='ligne ligneB'><div>Champ</div><div>Texte</div><div>Fond / Courbe</div><div>Bordure</div></div>";

            for (let i = 0; i < Koul.length; i++) {
                S += "<div class='ligne'>";
                S += "<div>" + Koul[i][0] + "</div>";
                S += "<div><input type='color' id='text_color" + i + "' value='#000000' onchange='readCouleur();'></div>";
                S += "<div><input type='color' id='bg_color" + i + "' value='#000000' onchange='readCouleur();'></div>";
                S += "<div><input type='color' id='bord_color" + i + "' value='#000000' onchange='readCouleur();'></div>";
                S += "</div>";
            }

            GH("colors", S);
        }

        function readCouleur() {
            for (let i = 0; i < Koul.length; i++) {
                if (Koul[i][1]) Koul[i][1] = GID("text_color" + i).value.substring(1);
                if (Koul[i][3]) Koul[i][3] = GID("bg_color" + i).value.substring(1);
                if (Koul[i][5]) Koul[i][5] = GID("bord_color" + i).value.substring(1);
            }
            CoulPage();
            setCouleur();
        }

        function CoulPage() {
            setColorQuery("body", "#" + Koul[0][1]);
            document.body.style.background =
                "linear-gradient(#" + Koul[0][5] + ",#" + Koul[0][3] + ",#" + Koul[0][5] + ")";
        }

        function SendValues(update) {
            let S = "?couleurs=";
            if(update){
              for (let i = 0; i < Koul.length; i++) {
                  if (Koul[i][1]) S += Koul[i][1];
                  if (Koul[i][3]) S += Koul[i][3];
                  if (Koul[i][5]) S += Koul[i][5];
              }
            }
            const xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState === 4 && this.status === 200) {
                    location.reload();
                }
            };

            xhttp.open("GET", "/CouleurUpdate" + S, true);
            xhttp.send();
        }

        function SetParaFixe() {
            GID("Bwifi").style.display = (F.ESP32_Type < 10) ? "inline-block" : "none";
            Set_Couleurs();
        }
    </script>

    <br>
    <div id="pied"></div>

    <script src="/ParaCommunJS"></script>
    <script src="/CommunCouleurJS"></script>
   

</body>
</html>

 )====";


const char *CommunCouleurJS = R"====(
 let Koul = []; // Couleurs courantes
let Coul_Page, Coul_Bout, Coul_W, Coul_VA, Coul_Wh, Coul_Tab, Coul_Graphe, Coul_Temp, Coul_Ouvre;

//------------------------------------------------------
// Etabli les couleurs 
//------------------------------------------------------
function Set_Couleurs() {
 
      let Retour = F.Couleurs; 

      if (Retour !== "") {
        let L = Retour.length;
        let j = 0;

        for (let i = 0; i < Koul.length; i++) {
          if (Koul[i][1] && j + 6 <= L) { Koul[i][1] = Retour.substring(j, j + 6); j += 6; }
          if (Koul[i][3] && j + 6 <= L) { Koul[i][3] = Retour.substring(j, j + 6); j += 6; }
          if (Koul[i][5] && j + 6 <= L) { Koul[i][5] = Retour.substring(j, j + 6); j += 6; }
        }
      }
      setCouleur();
  
}

//------------------------------------------------------
// Applique les couleurs partout dans la page
//------------------------------------------------------
function setCouleur() {

  for (let i = 0; i < Koul.length; i++) {

    // --- TEXTE ---
    if (Koul[i][1]) {
      let tc = GID("text_color" + i);
      if (tc) tc.value = "#" + Koul[i][1];

      if (Koul[i][2]) {
        for (let j = 0; j < Koul[i][2].length; j++) {
          setColorQuery(Koul[i][2][j], "#" + Koul[i][1]);
        }
      }
    } else {
      if (GID("text_color" + i))
        GID("text_color" + i).style.display = "none";
    }

    // --- FOND ---
    if (Koul[i][3]) {
      let bg = GID("bg_color" + i);
      if (bg) bg.value = "#" + Koul[i][3];

      if (Koul[i][4]) {
        for (let j = 0; j < Koul[i][4].length; j++) {
          setBgColorQuery(Koul[i][4][j], "#" + Koul[i][3]);
        }
      }
    } else {
      if (GID("bg_color" + i))
        GID("bg_color" + i).style.display = "none";
    }

    // --- BORDURE ---
    if (Koul[i][5]) {
      let bd = GID("bord_color" + i);
      if (bd) bd.value = "#" + Koul[i][5];

      if (Koul[i][6]) {
        for (let j = 0; j < Koul[i][6].length; j++) {
          setBoColorQuery(Koul[i][6][j], "#" + Koul[i][5]);
        }
      }
    } else {
      if (GID("bord_color" + i))
        GID("bord_color" + i).style.display = "none";
    }
  }

  //------------------------------------------------------
  // Inversion bordures (éclaircir)
  //------------------------------------------------------
  for (let i = 0; i < BordsInverse.length; i++) {
    let liste = document.querySelectorAll(BordsInverse[i]);

    for (let j = 0; j < liste.length; j++) {
      let rgb = liste[j].style.borderColor;

      if (!rgb || rgb.indexOf("(") < 0) continue;

      rgb = rgb.substring(rgb.indexOf("(") + 1, rgb.indexOf(")"));
      let tmp = rgb.split(",");

      if (tmp.length < 3) continue;

      let hexColor = "#";
      for (let k = 0; k < 3; k++) {
        let c = Math.min(255, Math.floor(tmp[k] * 1.8));
        let H = ("00" + c.toString(16)).slice(-2);
        hexColor += H;
      }
      liste[j].style.borderColor = hexColor;
    }
  }
}

//------------------------------------------------------
function setColorQuery(S, C) {
  document.querySelectorAll(S).forEach(e => e.style.color = C);
}

function setBgColorQuery(S, C) {
  document.querySelectorAll(S).forEach(e => e.style.background = C);
}

function setBoColorQuery(S, C) {
  document.querySelectorAll(S).forEach(e => e.style.borderColor = C);
}

//------------------------------------------------------
// Valeurs par défaut 
//------------------------------------------------------
function CouleurDefaut(){  
    let Coul=[];
    //Format: nom,CoulTexte,QueryTexte,CoulFond,QueryFond,CoulBorder,QueryBorder
    Coul_Page = Coul.length;
    Coul.push(["Page","*",,"*",,"*",]);
    Coul_Bout = Coul.length;
    Coul.push(["Boutons haut des pages","*",["a:visited","a:link"],"*",[".Bonglet",".Bonglet2"],"*",[".Bonglet",".Bonglet2"]]);
    Coul.push(["Champs de saisie","*",["input","select"],"*",["input","select"],"*",["input","select"]]);
    Coul_Tab = Coul.length;
    Coul.push(["Tableaux","*",[".tableau",".grid-container1",".grid-container2",".grid-container2M"],"*",[".tableau"],"*",[".tableau"]]);
    Coul_W = Coul.length;
    Coul.push(["Puissance Active en W",,,"*",[".W"],,]);
    Coul_VA = Coul.length;
    Coul.push(["Puissance Apparente en VA",,,"*",[".VA"],,]);
    Coul_Wh = Coul.length;
    Coul.push(["Energie Active en Wh",,,"*",[".Wh"],,]);
    Coul.push(["Volt V",,,"*",[".V"],,]);
    Coul.push(["Ampère A",,,"*",[".A"],,]);
    Coul.push(["Cosinus Phi",,,"*",[".phi"],,]);
    Coul.push(["Hertz et divers",,,"*",[".Hz",".Enph"],,]);
    Coul_Graphe = Coul.length;
    Coul.push(["Graphes","*",,"*",,"*",]);
    Coul_Temp = Coul.length;
    Coul.push(["Temperature Canal 0",,,"*",,,]);
    Coul.push(["Temperature Canal 1",,,"*",,,]);
    Coul.push(["Temperature Canal 2",,,"*",,,]);
    Coul.push(["Temperature Canal 3",,,"*",,,]);
    Coul_Ouvre = Coul.length; 
    Coul.push(["Ouverture SSR/Triac 0",,,"*",,,]);
    Coul.push(["Ouverture SSR/Triac 1",,,"*",,,]);
    Coul.push(["Ouverture SSR/Triac 2",,,"*",,,]);
    Coul.push(["Ouverture SSR/Triac 3",,,"*",,,]);
    Koul=Coul.slice(); //copie de travail
  }
CouleurDefaut();  // initialisation couleurs courantes

 )====";