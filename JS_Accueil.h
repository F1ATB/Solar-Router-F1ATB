//**********************************************
// Page principale  Javascript 
//**********************************************
const char * MainJS = R"====(
var tabPW2sM = [];
var tabPW2sT = [];
var tabActOuvre = [];
var LastPW_M = 0;
var LastPVA_M = 0;
var LastPW_T = 0;
var LastPVA_T = 0;
var LastActOuvre = [];
var PuisMaxS_M = 0;
var PuisMaxI_M = 0;
var PuisMaxS_T = 0;
var PuisMaxI_T = 0;
var initUxIx2 = false;
var TabVal = [];
var TabCoul = [];
var myTimeout;
var myActionTimeout;
var ActionForce = [];
var Pva_valide = false;
var BordsInverse = [".Baccueil"];
var DispAutres = [];
function LoadData() {
  GID('LED').style = 'display:block;';
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var DuRMS = this.responseText;
      var groupes = DuRMS.split(GS);
      var G0 = groupes[0].split(RS);
      var G1 = groupes[1].split(RS);
      var G2 = groupes[2].split(RS);
      GID('date').innerHTML = G0[1];
      Source_data = G0[2];
      if (!initUxIx2) {
        initUxIx2 = true;
        //if (groupes.length == 4 && nomSondeFixe != "") { // Cas pour les sources externes UxIx2 et Shelly monophasé Si deuxième canal nommé
        //   biSonde = true;
        // }
      }
      

      GID('PwS_M').innerHTML = LaVal(G1[0]); //Maison
      GID('PwI_M').innerHTML = LaVal(G1[1]); //Maison
      GID('PVAS_M').innerHTML = LaVal(G1[2]); //Maison
      GID('PVAI_M').innerHTML = LaVal(G1[3]); //Maison
      GID('EAJS_M').innerHTML = LaVal(G1[4]);
      GID('EAJI_M').innerHTML = LaVal(G1[5]);
      GID('EAS_M').innerHTML = LaVal(G1[6]);
      GID('EAI_M').innerHTML = LaVal(G1[7]);
      PuisMaxS_M = Math.max(PuisMaxS_M, parseInt(G1[0]));
      PuisMaxI_M = Math.max(PuisMaxI_M, parseInt(G1[1]));
      GID('PMS_M').innerHTML = LaVal(PuisMaxS_M);
      GID('PMI_M').innerHTML = LaVal(PuisMaxI_M);

      LastPW_M = parseFloat(G1[0] - G1[1]);
      LastPVA_M = parseFloat(G1[2] - G1[3]);


      var Tarif = ["NON_DEFINI", "PLEINE", "CREUSE", "BLEU", "BLANC", "ROUGE"];
      var couleur = ["#" + Koul[Coul_Tab][3], "#f00", "#0f0", "#00bfff", "#fff", "#f00"];
      var tarif = ["", "H.<br>pleine", "H.<br>creuse", "Tempo<br>Bleu", "Tempo<br>Blanc", "Tempo<br>Rouge"];
      var idx = 0;
      for (i = 0; i < 6; i++) {
        if (G0[3].indexOf(Tarif[i]) >= 0) { //LTARF dans Link
          idx = i;
        }
      }
      GID('couleurTarif_jour').style.backgroundColor = couleur[idx];
      GID('couleurTarif_jour').innerHTML = tarif[idx];
      var tempo = parseInt(G0[4], 16); //Tempo lendemain et jour STGEt
      tempo = Math.floor(tempo / 4); //Tempo lendemain uniquement
      idx = -2;
      var txtJ = "";
      if (tempo > 0) {
        idx = tempo;
        txtJ = "Tempo<br>J+1";
      }
      GID('couleurTarif_J1').style.backgroundColor = couleur[idx + 2];
      GID('couleurTarif_J1').innerHTML = txtJ;
      Pva_valide = (G0[6] == 1) ? true : false;

      if (groupes.length == 4 && nomSondeFixe != "") { // La source_data des données est de type UxIx2 ou on est en shelly monophas avec un deuxièeme canal
        GID('PwS_T').innerHTML = LaVal(G2[0]); //Triac
        GID('PwI_T').innerHTML = LaVal(G2[1]); //Triac
        GID('PVAS_T').innerHTML = LaVal(G2[2]); //Triac
        GID('PVAI_T').innerHTML = LaVal(G2[3]); //Triac
        GID('EAJS_T').innerHTML = LaVal(G2[4]);
        GID('EAJI_T').innerHTML = LaVal(G2[5]);
        GID('EAS_T').innerHTML = LaVal(G2[6]);
        GID('EAI_T').innerHTML = LaVal(G2[7]);
        PuisMaxS_T = Math.max(PuisMaxS_T, parseInt(G2[0]));
        PuisMaxI_T = Math.max(PuisMaxI_T, parseInt(G2[1]));
        GID('PMS_T').innerHTML = LaVal(PuisMaxS_T);
        GID('PMI_T').innerHTML = LaVal(PuisMaxI_T);

        LastPW_T = parseFloat(G2[0] - G2[1]);
        LastPVA_T = parseFloat(G2[2] - G2[3]);


        //  biSonde = true;
      }
      AdaptationSource();
      if (!Pva_valide) {
        const collection = document.getElementsByClassName('VA');
        for (let i = 0; i < collection.length; i++) {
          collection[i].style.display = "none";
        }
      }
      GID('LED').style = 'display:none;';

      setTimeout('LoadData();', 2000);
    }

  };
  xhttp.open('GET', '/ajax_data', true);
  xhttp.send();
}

function LoadHisto10mn() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var retour = this.responseText;
      var groupes = retour.split(GS);
      tabPW2sM.splice(0, tabPW2sM.length);
      tabPW2sM = groupes[1].split(',');
      tabPW2sM.pop();
      if (biSonde) {
        tabPW2sT.splice(0, tabPW2sT.length);
        tabPW2sT = groupes[2].split(',');
        tabPW2sT.pop();
      }
      groupes.shift(); groupes.shift(); groupes.shift();
      if (groupes.length > 0) { //Tableau des ouvertures par action
        for (var g = 0; g < groupes.length; g++) {
          var Act = groupes[g].split(ES);
          var i = parseInt(Act[0]); //Index de l'action
          tabActOuvre[i] = Act[1].split(RS);
          tabActOuvre[i].pop();
        }

      }
      LoadHisto1an();
    }

  };
  xhttp.open('GET', '/ajax_data10mn', true);
  xhttp.send();
}
function LoadHisto48h() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var retour = this.responseText;
      var groupes = retour.split(GS);
      var Pmaxi = groupes[0].split(RS);
      PuisMaxS_M = Pmaxi[0];
      PuisMaxI_M = Pmaxi[1];
      PuisMaxS_T = Pmaxi[2];
      PuisMaxI_T = Pmaxi[3];
      var tabPWM = groupes[1].split(',');
      tabPWM.pop();
      Plot('SVG_PW48hM', tabPWM, Koul[Coul_W][3], 'Puissance Active ' + nomSondeMobile + ' sur 48h en W', '', '');
      if (biSonde) {
        var tabPWT = groupes[2].split(',');
        tabPWT.pop();
        GID('SVG_PW48hT').style.display = "block";
        Plot('SVG_PW48hT', tabPWT, Koul[Coul_W][3], 'Puissance Active ' + nomSondeFixe + ' sur 48h en W', '', '');
      }
      groupes.shift(); groupes.shift(); groupes.shift();
      var lesTemp = groupes[0].split("|");
      for (var c = 0; c < 4; c++) {
        var tabTemperature = lesTemp[c].split(',');
        if (parseFloat(tabTemperature[tabTemperature.length - 1]) > -100) {
          tabTemperature.pop();
          GID('SVG_Temp48h' + c).style.display = "block";
          Plot('SVG_Temp48h' + c, tabTemperature, Koul[Coul_Temp + c][3], nomTemperature[c] + ' sur 48h ', '', '');
        }
      }
      groupes.shift();
      if (groupes.length > 0) {
        Plot_ouvertures(groupes);
      }
      setTimeout('LoadHisto48h();', 300000);
    }

  };
  xhttp.open('GET', '/ajax_histo48h', true);
  xhttp.send();
}
function LoadHisto1an() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var retour = this.responseText;
      var tabWh = retour.split(',');
      tabWh.pop();

      Plot('SVG_Wh1an', tabWh, Koul[Coul_Wh][3], 'Energie Active Wh / Jour sur 1an', '', '');
      LoadHisto48h();
    }

  };
  xhttp.open('GET', '/ajax_histo1an', true);
  xhttp.send();
}
function FinParaRouteur() {
  LoadCouleurs();
  GID("TabMesures").style.display = "block";
  LoadData();
  LoadHisto10mn();
  EtatActions(0, 0);
  var S = "";
  for (var C = 1; C < IP_RMS.length; C++) {  //Affichage autres routeurs
    S += "<div class='autreRMS'><div>" + IP_RMS[C] + "</div><div>" + nomRMS[C] + "</div><div id='autreRid" + C + "' onclick='autreRclick(" + C + ");' style='cursor:pointer;' ></div></div>";
    S += "<div class='autreRif' id='autreRif" + C + "'></div>";
  }
  GH("autresRMS", S);
  var IPextDisp = int2ip(RMSextIP);
  var IdsxSource = -1;
  for (var C = 1; C < IP_RMS.length; C++) {
    if (IP_RMS[C] == IPextDisp) IdsxSource = C;
    autreRaffiche(C);
  }
  var S = 'Source : ';
  if (Source == "Ext") {
    S += 'ESP distant ' + IPextDisp;
    if (IdsxSource > -1) S += " " + nomRMS[IdsxSource];
    GID("donneeDistante").style.display = "block";
  } else {
    S += 'ESP local';
  }
  GH('source', S);
}
function autreRclick(C) {
  var S = "";
  var fleche = "&#x21E7";
  if (DispAutres[C] == "true") {
    DispAutres[C] = "false";
  } else {
    DispAutres[C] = "true";
  }
  autreRaffiche(C);
}
function autreRaffiche(C) {
  var S = "";
  var fleche = "⬆️";
  if (DispAutres[C] == "true") {
    S += "<iframe src='http://" + IP_RMS[C] + "' ></iframe>";
  } else {
    fleche = "⬇️";
  }
  GH("autreRif" + C, S);
  GH("autreRid" + C, fleche);
  localStorage.setItem("DispAutre" + C, DispAutres[C]);
}

function Plot(SVG, Tab, couleur1, titre1, couleur2, titre2) {
  var Vmax = 0;
  var Vmin = 0;
  var TabY0 = [];
  var TabY1 = [];
  couleur1 = "#" + couleur1;
  couleur2 = "#" + couleur2;
  var dX = 900 / Tab.length;
  const d = new Date();
  var dI = 1;
  var label = 'heure';
  var pixelTic = 72;
  var dTextTic = 4;
  var moduloText = 24;
  var H0 = d.getHours() + d.getMinutes() / 60;
  var H00 = 4 * Math.floor(H0 / 4);
  var X0 = 18 * (H00 - H0);
  var Y0 = 250;
  var Yamp = 230;
  var dy = 2;
  var dispVA = false;
  switch (SVG) {
    case 'SVG_PW48hM':

      break;
    case 'SVG_PW48hT':

      break;
    case 'SVG_Temp48h':
      Y0 = 450;
      Yamp = 430;
      dy = 1;
      break;
    case 'SVG_PW2sM':
      label = 'mn';
      pixelTic = 90;
      X0 = 0;
      dTextTic = 1;
      moduloText = -100;
      H00 = 0;
      dI = 2; //2 courbes PW et PVA
      GID(SVG + '_L').style = 'color:' + couleur2 + ';display:block;';
      dispVA = GID(SVG + '_C').checked; //Plot courbe VA
      localStorage.setItem(SVG + '_LS', dispVA);
      break;
    case 'SVG_PW2sT':
      label = 'mn';
      pixelTic = 90;
      X0 = 0;
      dTextTic = 1;
      moduloText = -100;
      H00 = 0;
      dI = 2; //2 courbes PW et PVA
      GID(SVG + '_L').style = 'color:' + couleur2 + ';display:block;';
      dispVA = GID(SVG + '_C').checked; //Plot courbe VA
      localStorage.setItem(SVG + '_LS', dispVA);
      break;
    case 'SVG_Wh1an':
      label = 'Mois';
      pixelTic = dX * 30.4375;//Mois moyen
      var dTextTic = 1;
      moduloText = 12;
      H00 = d.getMonth();
      X0 = dX * (1 - d.getDate());
      var Mois = ['Jan', 'Fev', 'Mars', 'Avril', 'Mai', 'Juin', 'Juil', 'Ao&ucirc;t', 'Sept', 'Oct', 'Nov', 'Dec'];
      break;

  }
  for (var i = 0; i < Tab.length; i++) {
    if (i % 2 == 0 || dI == 1 || dispVA) {
      Tab[i] = Math.min(Tab[i], 10000000);
      Tab[i] = Math.max(Tab[i], -10000000);
      Vmax = Math.max(Math.abs(Tab[i]), Vmax);
    }
  }
  var cadrageMax = 1;
  var cadrage1 = 1000000;
  var cadrage2 = [10, 8, 5, 4, 2, 1];
  for (var m = 0; m < 7; m++) {
    for (var i = 0; i < cadrage2.length; i++) {
      var X = cadrage1 * cadrage2[i];
      if ((Vmax) <= X) cadrageMax = X;
    }
    cadrage1 = cadrage1 / 10;
  }

  var c1 = '"' + couleur1 + '"';
  var c2 = '"' + couleur2 + '"';
  var cT = "#" + Koul[Coul_Graphe][1];
  var style = 'background:linear-gradient( #' + Koul[Coul_Graphe][5] + ',#' + Koul[Coul_Graphe][3] + ',#' + Koul[Coul_Graphe][5] + ');border-color:#' + Koul[Coul_Tab][5] + ';';
  var S = "<svg viewbox='0 0 1030 500' style='" + style + "' height='500' width='100%' id='S_" + SVG + "' onmousemove ='DispVal(this,event);' >"; //   
  S += "<line x1='100' y1='20' x2='100' y2='480' style='stroke:" + cT + ";stroke-width:2' />";
  S += "<line x1='100' y1='" + Y0 + "' x2='1000' y2='" + Y0 + "' style='stroke:" + cT + ";stroke-width:2' />";

  for (var x = 1000 + X0; x > 100; x = x - pixelTic) {
    var X = x;
    var Y2 = Y0 + 6;
    S += "<line x1='" + X + "' y1='" + Y0 + "' x2='" + X + "' y2='" + Y2 + "' style='stroke:" + cT + ";stroke-width:2' />";
    X = X - 8;
    Y2 = Y0 + 22;
    if (SVG == 'SVG_Wh1an') {
      X = X + 8;
      S += "<text x='" + X + "' y='" + Y2 + "' style='font-size:16px;fill:" + cT + ";'>" + Mois[H00] + "</text>";
    } else {
      S += "<text x='" + X + "' y='" + Y2 + "' style='font-size:16px;fill:" + cT + ";'>" + H00 + "</text>";
    }
    H00 = (H00 - dTextTic + moduloText) % moduloText;
  }
  Y2 = Y0 - 3;
  S += "<text x='980' y='" + Y2 + "' style='font-size:14px;fill:" + cT + ";'>" + label + "</text>";
  for (var y = -10; y <= 10; y = y + dy) {

    Y2 = Y0 - Yamp * y / 10;
    if (Y2 <= 480) {
      S += "<line x1='100' y1='" + Y2 + "' x2='1000' y2='" + Y2 + "' style='stroke:" + cT + ";stroke-width:1;stroke-dasharray:2 10;' />";
      Y2 = Y2 + 7;
      var T = cadrageMax * y / 10; T = T.toString();
      var X = 90 - 9 * T.length;
      S += "<text x='" + X + "' y='" + Y2 + "' style='font-size:16px;fill:" + cT + ";'>" + T + "</text>";
    }
  }
  if (dI == 2 && Pva_valide && dispVA) { //Puissance apparente 
    S += "<text x='450' y='40' style='font-size:18px;fill:" + couleur2 + ";'>" + titre2 + "</text>";
    S += "<polyline points='";
    var j = 0;
    for (var i = 1; i < Tab.length; i = i + dI) {
      var Y = Y0 - Yamp * Tab[i] / cadrageMax;
      var X = 100 + dX * i;
      S += X + "," + Y + " ";
      TabY1[j] = parseFloat(Tab[i]);
      j++;
    }
    S += "' style='fill:none;stroke:" + couleur2 + ";stroke-width:2' />";
  }
  S += "<text x='450' y='18' style='font-size:18px;fill:" + couleur1 + ";'>" + titre1 + "</text>";
  S += "<polyline points='";
  var j = 0;
  for (var i = 0; i < Tab.length; i = i + dI) {
    var Y = Y0 - Yamp * Tab[i] / cadrageMax;
    var X = 100 + dX * i;
    S += X + "," + Y + " ";
    TabY0[j] = parseFloat(Tab[i]);
    j++;
  }
  S += "' style='fill:none;stroke:" + couleur1 + ";stroke-width:2' />";
  S += "</svg>";
  GID(SVG).innerHTML = S;
  TabVal["S_" + SVG] = [TabY0, TabY1]; //Sauvegarde valeurs
  TabCoul["S_" + SVG] = [couleur1, couleur2];

}

function DispVal(t, evt) {
  var ClientRect = t.getBoundingClientRect();
  var largeur_svg = ClientRect.right - ClientRect.left - 20; //20 pixels de marge
  var x = Math.round(evt.clientX - ClientRect.left - 10);
  x = x * 1030 / largeur_svg;
  var p = Math.floor((x - 100) * TabVal[t.id][0].length / 900);
  if (p >= 0 && p < TabVal[t.id][0].length) {
    var S = "";
    for (j = 0; j < TabVal[t.id].length; j++) {
      if (TabVal[t.id][j].length > 0) {
        S += "<div style='color:" + TabCoul[t.id][j] + ";'>" + TabVal[t.id][j][p] + "</div>";
      }
    }
    x = evt.pageX;
    GID("info").style.left = x + "px";
    x = ClientRect.top + 10 + window.scrollY;
    GID("info").style.top = x + "px";
    x = evt.pageY - x;
    GID("info_txt").style.top = x + "px";
    x = ClientRect.height - 20;
    GID("info").style.height = x + "px";
    GH("info_txt", S);
    GID("info").style.display = "block";
    if (myTimeout != null) clearTimeout(myTimeout);
    myTimeout = setTimeout(stopAffiche, 5000);
  }

}

function stopAffiche() {
  GID("info").style.display = "none";
}
function Plot_ouvertures(Gr) {
  GID("SVG_Ouvertures").style.display = "block";
  const d = new Date();
  var label = 'heure';
  var pixelTic = 72;
  var dTextTic = 4;
  var moduloText = 24;
  var H0 = d.getHours() + d.getMinutes() / 60;
  var H00 = 4 * Math.floor(H0 / 4);
  var X0 = 18 * (H00 - H0);
  var Hmax = 50 + 150 * Gr.length;
  var Y0 = Hmax - 50;
  var Couls = ["#" + Koul[Coul_Ouvre][3], "#" + Koul[Coul_Ouvre + 1][3], "#" + Koul[Coul_Ouvre + 2][3], "#" + Koul[Coul_Ouvre + 3][3]];
  var LesVals = [];
  var LesCouls = [];
  var cT = "#" + Koul[Coul_Graphe][1];
  var style = 'background:linear-gradient(#' + Koul[Coul_Graphe][5] + ',#' + Koul[Coul_Graphe][3] + ',#' + Koul[Coul_Graphe][5] + ');border-color:#' + Koul[Coul_Tab][5] + ';';
  var S = "<svg viewbox='0 0 1030 " + Hmax + "' height='" + Hmax + "'  style='" + style + "' width='100%' id='S_Ouvertures'  onmousemove ='DispVal(this,event);'>";
  S += "<line x1='100' y1='" + Y0 + "' x2='1000' y2='" + Y0 + "' style='stroke:" + cT + ";stroke-width:2' />";
  for (var x = 1000 + X0; x > 100; x = x - pixelTic) {
    var X = x;
    var Y2 = Y0 + 6;
    S += "<line x1='" + X + "' y1='" + Y0 + "' x2='" + X + "' y2='" + Y2 + "' style='stroke:" + cT + ";stroke-width:2' />";
    X = X - 8;
    Y2 = Y0 + 22;
    S += "<text x='" + X + "' y='" + Y2 + "' style='font-size:16px;fill:" + cT + ";'>" + H00 + "</text>";
    H00 = (H00 - dTextTic + moduloText) % moduloText;
  }
  Y2 = Y0 - 3;
  S += "<text x='980' y='" + Y2 + "' style='font-size:14px;fill:" + cT + ";'>" + label + "</text>";
  for (var i = 0; i < Gr.length; i++) {
    var tableau = Gr[i].split(RS);
    var Y00 = (i + 1) * 150;
    var Y2 = Y00 - 110;
    S += "<text x='450' y='" + Y2 + "' style='font-size:18px;fill:" + Couls[i % 4] + ";'>" + tableau.pop() + "</text>";
    S += "<line x1='100' y1='" + Y00 + "' x2='1000' y2='" + Y00 + "' style='stroke:" + cT + ";stroke-width:1;' />";
    Y2 = Y00 - 100;
    S += "<line x1='100' y1='" + Y2 + "' x2='1000' y2='" + Y2 + "' style='stroke:" + cT + ";stroke-width:1;stroke-dasharray:5 10;' />";
    Y2 = Y00 + 7;
    S += "<text x='80' y='" + Y2 + "' style='font-size:16px;fill:" + cT + ";'>0</text>";
    Y2 = Y00 - 93;
    S += "<text x='55' y='" + Y2 + "' style='font-size:16px;fill:" + cT + ";'>100%</text>";
    Y2 = Y00 - 100;
    S += "<line x1='100' y1='" + Y00 + "' x2='100' y2='" + Y2 + "' style='stroke:" + cT + ";stroke-width:1;' />";
    S += "<polyline points='";
    for (var j = 0; j < tableau.length; j++) {
      var Y = Y00 - tableau[j];
      var X = 100 + 1.5 * j;
      S += X + "," + Y + " ";
    }
    S += "' style='fill:none;stroke:" + Couls[i % 4] + ";stroke-width:2' />";

    LesVals.push(tableau); //Sauvegarde valeurs
    LesCouls.push(Couls[i % 4]);
  }
  S += "</svg>";
  TabVal["S_Ouvertures"] = LesVals;
  TabCoul["S_Ouvertures"] = LesCouls;
  GID("SVG_Ouvertures").innerHTML = S;

}
function Plot_ouvertures_2s() {
  var label = 'mn';
  var pixelTic = 90;
  var X0 = 0;
  var dTextTic = 1;
  var moduloText = -100;
  var H00 = 0;
  var Hmax = 50 + 150 * nomActions[0].length;
  var Y0 = Hmax - 50;
  var Couls = ["#" + Koul[Coul_Ouvre][3], "#" + Koul[Coul_Ouvre + 1][3], "#" + Koul[Coul_Ouvre + 2][3], "#" + Koul[Coul_Ouvre + 3][3]];
  var LesVals = [];
  var LesCouls = [];
  var cT = "#" + Koul[Coul_Graphe][1];
  var style = 'background:linear-gradient(#' + Koul[Coul_Graphe][5] + ',#' + Koul[Coul_Graphe][3] + ',#' + Koul[Coul_Graphe][5] + ');border-color:#' + Koul[Coul_Tab][5] + ';';
  var S = "<svg viewbox='0 0 1030 " + Hmax + "' height='" + Hmax + "'  style='" + style + "' width='100%' id='S_Ouvertures_2s'  onmousemove ='DispVal(this,event);'>";
  S += "<line x1='100' y1='" + Y0 + "' x2='1000' y2='" + Y0 + "' style='stroke:" + cT + ";stroke-width:2' />";
  for (var x = 1000 + X0; x > 100; x = x - pixelTic) {
    var X = x;
    var Y2 = Y0 + 6;
    S += "<line x1='" + X + "' y1='" + Y0 + "' x2='" + X + "' y2='" + Y2 + "' style='stroke:" + cT + ";stroke-width:2' />";
    X = X - 8;
    Y2 = Y0 + 22;
    S += "<text x='" + X + "' y='" + Y2 + "' style='font-size:16px;fill:" + cT + ";'>" + H00 + "</text>";
    H00 = (H00 - dTextTic + moduloText) % moduloText;
  }
  Y2 = Y0 - 3;
  S += "<text x='980' y='" + Y2 + "' style='font-size:14px;fill:" + cT + ";'>" + label + "</text>";
  for (var i = 0; i < nomActions[0].length; i++) {
    GID("SVG_Ouvertures_2s").style.display = "block";
    var idxAction = parseInt(nomActions[0][i][0]);
    if (tabActOuvre[idxAction].length > 0) tabActOuvre[idxAction].shift();
    tabActOuvre[idxAction].push(LastActOuvre[idxAction]);
    var Y00 = (i + 1) * 150;
    var Y2 = Y00 - 110;
    S += "<text x='450' y='" + Y2 + "' style='font-size:18px;fill:" + Couls[i % 4] + ";'>" + nomActions[0][i][1] + "</text>";
    S += "<line x1='100' y1='" + Y00 + "' x2='1000' y2='" + Y00 + "' style='stroke:" + cT + ";stroke-width:1;' />";
    Y2 = Y00 - 100;
    S += "<line x1='100' y1='" + Y2 + "' x2='1000' y2='" + Y2 + "' style='stroke:" + cT + ";stroke-width:1;stroke-dasharray:5 10;' />";
    Y2 = Y00 + 7;
    S += "<text x='80' y='" + Y2 + "' style='font-size:16px;fill:" + cT + ";'>0</text>";
    Y2 = Y00 - 93;
    S += "<text x='55' y='" + Y2 + "' style='font-size:16px;fill:" + cT + ";'>100%</text>";
    Y2 = Y00 - 100;
    S += "<line x1='100' y1='" + Y00 + "' x2='100' y2='" + Y2 + "' style='stroke:" + cT + ";stroke-width:1;' />";
    if (tabActOuvre[idxAction].length > 0) {
      S += "<polyline points='";
      for (var j = 0; j < tabActOuvre[idxAction].length; j++) {
        var Y = Y00 - tabActOuvre[idxAction][j];
        var X = 100 + 3 * j;
        S += X + "," + Y + " ";
      }
      S += "' style='fill:none;stroke:" + Couls[i % 4] + ";stroke-width:2' />";
      LesVals.push(tabActOuvre[idxAction]); //Sauvegarde valeurs
      LesCouls.push(Couls[i % 4]);
    }
  }
  S += "</svg>";
  TabVal["S_Ouvertures_2s"] = LesVals;
  TabCoul["S_Ouvertures_2s"] = LesCouls;
  GID("SVG_Ouvertures_2s").innerHTML = S;

}
function EtatActions(Force, NumAction) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var retour = this.responseText;
      var message = retour.split(GS);
      var LesTemp = message[0].split("|");
      Source_data = message[1];
      var T = "";
      for (var c = 0; c < 4; c++) {
        if (LesTemp[c] > -100) {
          var Temper = parseFloat(LesTemp[c]).toFixed(1);
          T += "<div class='item_temp_nom ce'>" + nomTemperature[c] + "</div><div class='item_temp_val ce'>" + Temper + "°C</div>";
        }
      }
      var S = "";
      if (message[3] > 0) { //Nb Actions 
        ActionForce.splice(0, ActionForce.length);
        for (var i = 0; i < message[3]; i++) {
          var data = message[i + 4].split(RS);
          ActionForce[i] = data[3];
          var ouvre = 0;
          S += "<div class='item_val ce'>" + data[1] + "</div>";
          if (data[2] == "On" || data[2] == "Off") {
            S += "<div class='item_val ce'>" + data[2] + "</div>";
            if (data[2] == "On") ouvre = 100;
          } else {
            var W = 1 + 1.99 * data[2];
            S += "<div class='item_val ce'><div class='jaugeBack item_val'><div class='jauge' style='width:" + W + "px'></div><div class='ce w100'>" + data[2] + "%</div></div></div>";
            ouvre = parseInt(data[2]);
          }
          LastActOuvre[parseInt(data[0])] = ouvre;
          S += "<div class='ce item_val'>" + Hdeci2Hmn(data[4]) + "</div>";
          var stOn = (ActionForce[i] > 0) ? "style='background-color:#f66;'" : "";
          var stOff = (ActionForce[i] < 0) ? "style='background-color:#f66;'" : "";
          var min = (ActionForce[i] == 0) ? "&nbsp;&nbsp;" : Math.abs(ActionForce[i]) + " min";
          S += "<div class='item_F ce'><input type='button' onclick='Force(" + data[0] + ",1);' value='On' " + stOn + "></div><div class='item_F ce'><small>" + min + "</small></div><div class='item_F ce'><input type='button' onclick='Force(" + data[0] + ",-1);' value='Off' " + stOff + "></div>";
        }
      }
      S = S + T;
      if (S != "") {
        S = "<div class='item_Act ce'>Etat Action(s)</div><div class='item_H ce'>H<div class='fsize10'>ouverture équivalente</div></div><div class='item_Force ce'> Forçage</div>" + S;
        GH("etatActions", S);
        ("th", "#" + Koul[Coul_Tab][5]);
        setBoColorQuery("td", "#" + Koul[Coul_Tab][5]);
        GID("etatActions").style.display = "grid";
      }
      myActionTimeout = setTimeout('EtatActions(0,0);', 3500);
    }

  };
  xhttp.open('GET', '/ajax_etatActions?Force=' + Force + '&NumAction=' + NumAction, true);
  xhttp.send();
}
function Refresh_2s() { //Rafraichit exactement toutes les 2s les graphiques de 10mn avec les dernieres valeur connues
  if (tabPW2sM.length > 0) {
    tabPW2sM.shift(); //Enleve Pw Maison
    tabPW2sM.shift(); //Enleve PVA
    tabPW2sM.push(LastPW_M);
    tabPW2sM.push(LastPVA_M);
    tabPW2sT.shift(); //Enleve Pw Triac
    tabPW2sT.shift(); //Enleve PVA
    tabPW2sT.push(LastPW_T);
    tabPW2sT.push(LastPVA_T);
    GH("nomSondeMobile", nomSondeMobile);
    if (biSonde) GH("nomSondeFixe", nomSondeFixe)
    var Nom_simul = nomSondeMobile;
    if (Source_data == "NotDef") Nom_simul = "(Puissance simulée. Source inconnue)";
    Plot('SVG_PW2sM', tabPW2sM, Koul[Coul_W][3], 'Puissance Active ' + Nom_simul + ' sur 10 mn en W', Koul[Coul_VA][3], 'Puissance Apparente sur 10 mn en VA');
    if (biSonde) {
      if (Source_data != "NotDef") Nom_simul = GID("nomSondeFixe").innerHTML;
      GID('SVG_PW2sT').style.display = "block";
      Plot('SVG_PW2sT', tabPW2sT, Koul[Coul_W][3], 'Puissance Active ' + Nom_simul + ' sur 10 mn en W', Koul[Coul_VA][3], 'Puissance Apparente sur 10 mn en VA');
    }
    if (tabActOuvre.length > 0) Plot_ouvertures_2s();
  }
}

function LaVal(d) {
  d = parseInt(d);
  d = '           ' + d.toString();
  return d.substr(-9, 3) + ' ' + d.substr(-6, 3) + ' ' + d.substr(-3, 3);
}
function Force(NumAction, Force) {
  if (myActionTimeout != null) clearTimeout(myActionTimeout);
  EtatActions(Force, NumAction);
}

function AdaptationSource() {

  if (biSonde && Source != "ShellyEm" && Source != "ShellyPro") { //Il n'y a pas d'injecté normalement
    GID('produite').innerHTML = '';
    GID('PwI_T').innerHTML = '';
    GID('PVAI_T').innerHTML = '';
    GID('EAJI_T').innerHTML = '';
    GID('EAI_T').innerHTML = '';
    GID('PMI_T').innerHTML = '';
    const collection = document.getElementsByClassName('grid-container2M');
        for (let i = 0; i < collection.length; i++) {
          collection[i].style.gridTemplateColumns = "auto auto auto auto 0px auto";
        }
   
  }

}
function Init() {
  SetHautBas();

  var VA_M = "true";
  var VA_T = "true";
  for (var i = 0; i < 8; i++) {
    DispAutres[i] = false;
  }
  if (localStorage.getItem("SVG_PW2sM_LS") !== null) { //Plot ou pas les VA
    VA_M = localStorage.getItem("SVG_PW2sM_LS");
    VA_T = localStorage.getItem("SVG_PW2sT_LS");
    for (var i = 0; i < 8; i++) {
      DispAutres[i] = localStorage.getItem("DispAutre" + i);
    }
  }

  if (biSonde) {


    var S = '<div class="grid-container2M">';
    S += '<div class="item1 ce" id="nomSondeMobile">Maison</div>';
    S += '<div class="item2 ce" id="nomSondeFixe">Triac/SSR</div>';
    S += '<div class="item3 ce" id="couleurTarif_jour">Tarif</div>';

    S += '<div class="item4 ce">Soutirée</div>';
    S += '<div class="item5 ce">Injectée</div>';
    S += '<div class="item6 ce">Conso.</div>';
    S += '<div class="item7 ce" id="produite">Produite</div>';
    S += '<div id="couleurTarif_J1" class="item14 ce">Tarif J1</div>';

    S += '<div class="item8 W">Puissance Active <small>(Pw)</small></div>';
    S += '<div class="item9 W" id="PwS_M"></div>';
    S += '<div class="item10 W" id="PwI_M"></div>';
    S += '<div class="item11 W" id="PwS_T"></div>';
    S += '<div class="item12 W" id="PwI_T"></div>';
    S += '<div class="item13 W">W</div>';

    S += '<div class="VA le">Puissance Apparente</div>';
    S += '<div class="VA" id="PVAS_M"></div>';
    S += '<div class="VA" id="PVAI_M"></div>';
    S += '<div class="VA" id="PVAS_T"></div>';
    S += '<div class="VA" id="PVAI_T"></div>';
    S += '<div class="VA">VA</div>';

    S += '<div class="W le">Puissance Active Max du jour</div>';
    S += '<div class="W" id="PMS_M"></div>';
    S += '<div class="W" id="PMI_M"></div>';
    S += '<div class="W" id="PMS_T"></div>';
    S += '<div class="W" id="PMI_T"></div>';
    S += '<div class="W">W</div>';

    S += '<div class="Wh le">Energie Active du jour</div>';
    S += '<div class="Wh" id="EAJS_M"></div>';
    S += '<div class="Wh" id="EAJI_M"></div>';
    S += '<div class="Wh" id="EAJS_T"></div>';
    S += '<div class="Wh" id="EAJI_T"></div>';
    S += '<div class="Wh">Wh</div>';

    S += '<div class="Wh le">Energie Active Totale</div>';
    S += '<div class="Wh" id="EAS_M"></div>';
    S += '<div class="Wh" id="EAI_M"></div>';
    S += '<div class="Wh" id="EAS_T"></div>';
    S += '<div class="Wh" id="EAI_T"></div>';
    S += '<div class="Wh">Wh</div>';
    S += '</div>';

  } else {

    var S = '<div class="grid-container1">';
    S += '<div class="item1 ce" id="nomSondeMobile">Maison</div>';
    S += '<div class="item31 ce" id="couleurTarif_jour">Tarif</div>';

    S += '<div class="item4 ce">Soutirée</div>';
    S += '<div class="item5 ce">Injectée</div>';
    S += '<div id="couleurTarif_J1" class="item14 ce">Tarif J1</div>';

    S += '<div class="item8 W">Puissance Active <small>(Pw)</small></div>';
    S += '<div class="item9 W" id="PwS_M"></div>';
    S += '<div class="item10 W" id="PwI_M"></div>';
    S += '<div class="item13 W">W</div>';

    S += '<div class="VA le">Puissance Apparente</div>';
    S += '<div class="VA" id="PVAS_M"></div>';
    S += '<div class="VA" id="PVAI_M"></div>';
    S += '<div class="VA">VA</div>';

    S += '<div class="W le">Puissance Active Max du jour</div>';
    S += '<div class="W" id="PMS_M"></div>';
    S += '<div class="W" id="PMI_M"></div>';
    S += '<div class="W">W</div>';

    S += '<div class="Wh le">Energie Active du jour</div>';
    S += '<div class="Wh" id="EAJS_M"></div>';
    S += '<div class="Wh" id="EAJI_M"></div>';
    S += '<div class="Wh">Wh</div>';

    S += '<div class="Wh le">Energie Active Totale</div>';
    S += '<div class="Wh" id="EAS_M"></div>';
    S += '<div class="Wh" id="EAI_M"></div>';
    S += '<div class="Wh">Wh</div>';
    S += '</div>';
  }
  GH("TabMesures", S);




  LoadParaRouteur();
  GID("SVG_PW2sM_C").checked = (VA_M == "true") ? true : false;
  GID("SVG_PW2sT_C").checked = (VA_T == "true") ? true : false;
  setInterval(Refresh_2s, 2000);

}
)====";
// icône panneaux solaire et soleil (Merci michy)
const char * Favicon = R"====(
  <svg xmlns = "http://www.w3.org/2000/svg" width = "64" height = "64" viewBox = "15 20 42 40" >
<path d="m16 36h19l16 20h-20z" fill="#92d3f5"></path>
<circle cx="48" cy="32" fill="#fcea2b" r="8"></circle>
<g fill="none" stroke="#000" stroke-linecap="round" stroke-linejoin="round" stroke-width="2">
  <path d="m16 36h19l16 20h-20z"></path><path d="m17 43v13h10"></path>
  <path d="m24 46h19"></path><path d="m26 36 16 20"></path>
  <circle cx="48" cy="32" r="8"></circle>
</g>
</svg >
)====";
