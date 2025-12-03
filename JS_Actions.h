const char * ActionsJS1 = R"====(
var LesActions = [];
var mouseClick = false;
var blockEvent = false;
var LesTemperatures = [];
var NomTemperatures = [];
var ListeActions = [];
var SelectActions = "";
var LTARFbin = 0;
var pTriac = 0;
var ReacCACSI = 1;
var PlotIdx = -1;
var TabIdx = 0;
var MesuresPwAct = [];
var Ecart = [];
var Ouvert = [];
var Retard = [];
var Prop = [];
var Integ = [];
var Deriv = [];
var Pause = false;
var IS = "|"; //Input Separator
var BordsInverse = [".Bactions"];
function Init() {
    LoadActions();
    LoadCouleurs();
    ShowAction();
}
function creerAction(aActif, aTitre, aHost, aPort, aOrdreOn, aOrdreOff, aRepet, aTempo, aKp, aKi, aKd, aPID, aPeriodes) {
    if (aTitre == "") aTitre = "Titre";
    var S = {
        Actif: aActif,
        Titre: aTitre,
        Host: aHost,
        Port: aPort,
        OrdreOn: aOrdreOn,
        OrdreOff: aOrdreOff,
        Repet: aRepet,
        Tempo: aTempo,
        Kp: aKp,
        Ki: aKi,
        Kd: aKd,
        PID: aPID,
        Periodes: aPeriodes
    }
    return S;
}
function TracePlanning(iAct) {
    var Radio0 = "<div ><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-0' onclick='checkDisabled();'>Inactif</div>";
    var Radio1 = "<div ><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-1'  onclick='checkDisabled();'>Découpe sinus</div>";
    if (iAct > 0) { Radio1 = "<div ><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-1'  onclick='checkDisabled();'>On/Off</div>"; }
    Radio1 += "<div ><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-5'  onclick='checkDisabled();'>Demi-sinus</div>";
    Radio1 += "<div ><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-2'  onclick='checkDisabled();'>Multi-sinus</div>";
    Radio1 += "<div ><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-3'  onclick='checkDisabled();'>Train de sinus</div>";
    Radio1 += "<div id='Pwm" + iAct + "'><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-4'  onclick='checkDisabled();'>PWM</div>";
    var SelectPin = "<div>Gpio <select id='selectPin" + iAct + "' onchange='checkDisabled();'  title='Choix broche (GPIO) de commande'>";
    for (var i = 0; i < Pins.length; i++) {
        var v = "gpio:" + Pins[i];
        if (Pins[i] == 0) v = "";
        if (Pins[i] == -1) v = "Externe";
        SelectPin += "<option value=" + Pins[i] + ">" + v + "</option>";
    }
    SelectPin += "</select></div>";
    var SelectOut = "<div id='SelectOut" + iAct + "'>Sortie 'On' <select id='selectOut" + iAct + "'  title='Dépend du relais. En général +3.3V'><option value=0>0V</option><option value=1 selected>3.3V</option></select></div>";

    var S = "<div class='titre'><span id ='titre" + iAct + "'  onclick='editTitre(" + iAct + ")' title='Donnez un nom'>Titre</span></div>";
    S += "<div class='visu' onclick='Plot(" + iAct + ")' id='visu" + iAct + "' title='Zoom sur la régulation en temps réel. Les réglages peuvent être modifiés. Ne pas oublier de sauvegarder'>&#128200;</div>";
    S += "<div  id='mode' ><div class='TitZone' title='Choix du mode de découpe du secteur'>Mode</div>" + Radio0 + Radio1 + "</div>";
    S += "<div id='blocPlanning" + iAct + "' >";
    S += "<div class='les_select' id='sortie" + iAct + "'>";
    S += "<div class='TitZone' title='Définir la broche (GPIO) ou commande' >Sortie</div>" + SelectPin + SelectOut;
    S += "<div><span id='Tempo" + iAct + "'>Temporisation(s) <input type='number' class='tm' id='tempo" + iAct + "'  title='Temporisation entre chaque changement d&apos;état pour éviter des oscillations quand un appareil dans la maison consomme en dents de scie (Ex: un four).'></span></div>";
    S += "</div><div class='les_select' id='ligne_bas" + iAct + "'><div class='TitZone'>Externe</div>";
    S += "<div><span id='Host" + iAct + "'>Host<br><input type='text' id='host" + iAct + "'  onchange='checkDisabled();' title='Adresse IP machine sur réseau LAN, nom de domaine ou rien.'></span></div>";
    S += "<div><span id='Port" + iAct + "'>Port<br><input type='number' class='tm' id='port" + iAct + "'  title='Port d&apos;acc&egrave;s via le protocole http , uniquement pour machine distante. En g&eacute;n&eacute;ral <b>80</b>.'></span></div>";
    S += "<div><span id='ordreon" + iAct + "'>Ordre On<br><input type='text' id='ordreOn" + iAct + "'  title='Ordre à passer à la machine distante.'></span></div>";
    S += "<div><span id='ordreoff" + iAct + "'>Ordre Off<br><input type='text' id='ordreOff" + iAct + "' ></span></div>";
    S += "<div><span id='Repet" + iAct + "'>Répétition(s)<br><input type='number' id='repet" + iAct + "' class='tm'  title='P&eacute;riode en s de r&eacute;p&eacute;tition/rafra&icirc;chissement de la commande. Uniquement pour les commandes vers l&apos;extérieur. 0= pas de répétition.'></span></div>";
    S += "</div>";

    S += "<div  class='bouton_curseur' ><div class='boutons'><input id='adds' type='button' value='-' class='tbut'  onclick='AddSub(-1," + iAct + ")'  title='Retrait d&apos;une p&eacute;riode horaire.'>";
    S += "<input id='adds' type='button' value='+' class='tbut' onclick='AddSub(1," + iAct + ")' title='Ajout d&apos;une p&eacute;riode horaire.'></div>";
    S += "<div class='slideTriac' id='fen_slide" + iAct + "'>";
    S += "<div class='slideTriacIn' id='Propor" + iAct + "'>";
    S += "<div class='Tcell1'>Coef. Proportionnel</div>";
    S += "<div class='Tcell2'><input type='range' min='0' max='100' value='50' title='Correction proportionnelle à l&apos;écart en W' id='sliderKp" + iAct + "' style='width:100%;max-width:none;' oninput=\"GH('sensiKp" + iAct + "',Math.floor(this.value));UpdateK(" + iAct + ");\"  ></div>";
    S += "<div class='Tcell3'><strong id='sensiKp" + iAct + "'></strong></div>";
    S += "</div>";
    S += "<div class='slideTriacIn'>";
    S += "<div class='Tcell1'>Coef. Intégral ou R&eacute;activit&eacute; </div>";
    S += "<div class='Tcell2'><input type='range' min='0' max='100' value='50' title='Correction suivant l&apos;intégrale de l&apos;écart en W' id='sliderKi" + iAct + "'  style='width:100%;max-width:none;' oninput=\"GH('sensiKi" + iAct + "',Math.floor(this.value));UpdateK(" + iAct + ");\"  ></div>";
    S += "<div class='Tcell3'><strong id='sensiKi" + iAct + "'></strong></div>";
    S += "</div>";
    S += "<div class='slideTriacIn' title='Correction suivant la derivée de l&apos;écart en W' id='Derive" + iAct + "'>";
    S += "<div class='Tcell1'>Coef. Dérivé</div>";
    S += "<div class='Tcell2'><input type='range' min='0' max='100' value='50' id='sliderKd" + iAct + "' style='width:100%;max-width:none;' oninput=\"GH('sensiKd" + iAct + "',Math.floor(this.value));UpdateK(" + iAct + ");\"  ></div>";
    S += "<div class='Tcell3'><strong id='sensiKd" + iAct + "'></strong></div>";
    S += "</div>";
    S += "</div>";
    S += "<div id='PIDbox" + iAct + "'><label> PID On</label><input type='checkbox' id='PID" + iAct + "' onclick='checkDisabled();'></div>";
    S += "</div>";
    S += "<did id='graphAction" + iAct + "' class='graphAction'><div id='graphSVG" + iAct + "' class='graphSVG'></div><div class='GraphSVG' onclick='Pause=!Pause;'>&#x23EF;</div></div>";
    S += "<div style='margin:4px;'>";
    S += "<div id='infoAction" + iAct + "' class='infoAction'></div>";
    S += "<div id='curseurs" + iAct + "' class='curseur'  onmousedown='mouseClick=true;'  onmousemove='mouseMove(this,event," + iAct + ");'  ontouchstart='touchMove(this,event," + iAct + ");'  ontouchmove='touchMove(this,event," + iAct + ");' ></div>";
    S += "</div>";
    S += "</div>";

    GH("planning" + iAct, S);
    GID("radio" + iAct + "-" + LesActions[iAct].Actif).checked = true;
    GH("titre" + iAct, LesActions[iAct].Titre);
    GV("host" + iAct, LesActions[iAct].Host);
    GV("port" + iAct, LesActions[iAct].Port);
    GV("ordreOn" + iAct, LesActions[iAct].OrdreOn);
    GV("ordreOff" + iAct, LesActions[iAct].OrdreOff);
    GV("repet" + iAct, LesActions[iAct].Repet);
    GV("tempo" + iAct, LesActions[iAct].Tempo);
    GV("sliderKp" + iAct, LesActions[iAct].Kp);
    GH("sensiKp" + iAct, LesActions[iAct].Kp);
    GV("sliderKi" + iAct, LesActions[iAct].Ki);
    GH("sensiKi" + iAct, LesActions[iAct].Ki);
    GV("sliderKd" + iAct, LesActions[iAct].Kd);
    GH("sensiKd" + iAct, LesActions[iAct].Kd);
    GID("PID" + iAct).checked = LesActions[iAct].PID == 1 ? true : false;
    if (LesActions[iAct].OrdreOn.indexOf(IS) > 0) {
        var vals = LesActions[iAct].OrdreOn.split(IS);
        GID("selectPin" + iAct).value = vals[0];
        GID("selectOut" + iAct).value = vals[1];
    } else {
        GID("selectPin" + iAct).value = -1;
        GID("selectOut" + iAct).value = 1;
        if (LesActions[iAct].OrdreOn == "") GID("selectPin" + iAct).value = 0;
    }
    TracePeriodes(iAct);

}


function TracePeriodes(iAct) {
    var S = "";
    var Sinfo = "";
    var SinfoClick = "";
    var left = 0;
    var H0 = 0;
    var colors = ["#666", "#66f", "#f66", "#6f6", "#cc4"]; //NO,OFF,ON,PW,Triac
    blockEvent = false;
    for (var i = 0; i < LesActions[iAct].Periodes.length; i++) {
        var w = (LesActions[iAct].Periodes[i].Hfin - H0) / 24;
        left = H0 / 24;
        H0 = LesActions[iAct].Periodes[i].Hfin;
        var Type = LesActions[iAct].Periodes[i].Type;
        var color = colors[Type];
        var temperature = "";
        if (LesActions[iAct].Periodes[i].CanalTemp >= 0) {
            if (LesTemperatures[LesActions[iAct].Periodes[i].CanalTemp] > -100) { // La sonde de température fonctionne          
                var Tsup = LesActions[iAct].Periodes[i].Tsup;
                if (Tsup >= 0 && Tsup <= 1000) temperature += "<div>T &ge;" + Tsup / 10 + "°</div>";
                var Tinf = LesActions[iAct].Periodes[i].Tinf;
                if (Tinf >= 0 && Tinf <= 1000) temperature += "<div>T &le;" + Tinf / 10 + "°</div>";
            }
        }
        var H_Ouvert = "";
        if (LesActions[iAct].Periodes[i].SelAct != 255) {
            if (LesActions[iAct].Periodes[i].Hmin > 0) H_Ouvert += "<div>H<span class='fsize8'>ouverture</span> &ge;" + Hdeci2Hmn(LesActions[iAct].Periodes[i].Hmin) + "</div>";
            if (LesActions[iAct].Periodes[i].Hmax > 0) H_Ouvert += "<div>H<span class='fsize8'>ouverture</span> &le;" + Hdeci2Hmn(LesActions[iAct].Periodes[i].Hmax) + "</div>";
            if (LesActions[iAct].Periodes[i].Ooff > 0) H_Ouvert += "<div>On à Off si &le;" + LesActions[iAct].Periodes[i].Ooff + "%</div>";
            if (LesActions[iAct].Periodes[i].O_on > 0) H_Ouvert += "<div>Off à On si &ge;" + LesActions[iAct].Periodes[i].O_on + "%</div>";
        }
        var TxtTarif = "";
        if (LTARFbin > 0) {
            TxtTarif = "Tarif : ";
            var Tarif_ = LesActions[iAct].Periodes[i].Tarif;
            if (LTARFbin <= 3) {
                TxtTarif += (Tarif_ & 1) ? "<span style='color:red;'>H. Pleine</span>" : "";
                TxtTarif += (Tarif_ & 2) ? "<span style='color:green;'> H. Creuse</span>" : "";
            } else {
                TxtTarif += (Tarif_ & 4) ? "Tempo<span style='color:blue;'>Bleu</span>" : "";
                TxtTarif += (Tarif_ & 8) ? "<span style='color:white;'> Blanc</span>" : "";
                TxtTarif += (Tarif_ & 16) ? "<span style='color:red;'> Rouge</span>" : "";
            }
            TxtTarif = "<div>" + TxtTarif + "</div>";
        }
        let TexteMinMax = "";
        var condition = (temperature != "" || H_Ouvert != "" || TxtTarif != "") ? "<div>Condition(s) :</div>" + temperature + H_Ouvert + TxtTarif : "";
        if (LesActions[iAct].Actif <= 1 && iAct > 0) {
            LesActions[iAct].Periodes[i].Vmax = Math.max(LesActions[iAct].Periodes[i].Vmin, LesActions[iAct].Periodes[i].Vmax);
            TexteMinMax = "<div>Off si Pw&gt;" + LesActions[iAct].Periodes[i].Vmax + "W</div><div>On si Pw&lt;" + LesActions[iAct].Periodes[i].Vmin + "W</div>" + condition;
        } else {
            LesActions[iAct].Periodes[i].Vmax = Math.max(0, LesActions[iAct].Periodes[i].Vmax);
            LesActions[iAct].Periodes[i].Vmax = Math.min(100, LesActions[iAct].Periodes[i].Vmax);
            TexteMinMax = "<div>Seuil Pw : " + LesActions[iAct].Periodes[i].Vmin + "W</div>" + "<div>Ouvre Max : " + LesActions[iAct].Periodes[i].Vmax + "%</div>" + condition;
        }
        var TexteTriac = "<div>Seuil Pw : " + LesActions[iAct].Periodes[i].Vmin + "W</div>" + "<div>Ouvre Max : " + LesActions[iAct].Periodes[i].Vmax + "%</div>" + condition;
        var paras = ["Pas de contr&ocirc;le", "OFF", "<div>ON</div>" + condition, TexteMinMax, TexteTriac];
        var para = paras[Type];
        S += "<div class='periode' style='width:" + w + "%;left:" + left + "%;background-color:" + color + ";'   ><div>&lArr;</div><div>&rArr;</div></div>";
        Hmn = Hdeci2Hmn(H0);
        fs = Math.max(8, Math.min(16, w / 2)) + "px";
        Sinfo += "<div class='infoZone' style='width:" + w + "%;border-color:" + color + ";font-size:" + fs + "'  onclick='infoZclicK(" + i + "," + iAct + ")'  >"
        Sinfo += "<div class='Hfin'>" + Hmn + "</div>" + para + "</div>";
        SinfoClick += "<div id='info" + iAct + "Z" + i + "' class='infoZ' ></div>";
    }
    GH("curseurs" + iAct, S);
    GH("infoAction" + iAct, SinfoClick + Sinfo);
}

)====";

const char * ActionsJS2 = R"====(
    
function touchMove(t, ev, iAct) {
    var leftPos = ev.touches[0].clientX - GID(t.id).getBoundingClientRect().left;
    NewPosition(t, leftPos, iAct);
}
function mouseMove(t, ev, iAct) {
    if (mouseClick) {
        var leftPos = ev.clientX - GID(t.id).getBoundingClientRect().left;
        NewPosition(t, leftPos, iAct);
    }
}


function NewPosition(t, leftPos, iAct) {
    var G = GID(t.id).style.left;
    //+ window.scrollX;
    var width = GID(t.id).getBoundingClientRect().width;
    var HeureMouse = leftPos * 2420 / width;
    var idxClick = 0;
    var deltaX = 999999;
    for (var i = 0; i < LesActions[iAct].Periodes.length - 1; i++) {
        let dist = Math.abs(HeureMouse - LesActions[iAct].Periodes[i].Hfin)
        if (dist < deltaX) {
            idxClick = i;
            deltaX = dist;
        }
    }
    var NewHfin = Math.max(0, Math.min(HeureMouse, 2400));
    if (idxClick == LesActions[iAct].Periodes.length - 1) NewHfin = 2400;
    if (idxClick < LesActions[iAct].Periodes.length - 1)
        NewHfin = Math.min(NewHfin, LesActions[iAct].Periodes[idxClick + 1].Hfin);
    if (idxClick > 0)
        NewHfin = Math.max(NewHfin, LesActions[iAct].Periodes[idxClick - 1].Hfin);
    LesActions[iAct].Periodes[idxClick].Hfin = Math.floor(NewHfin);
    TracePeriodes(iAct);

}


function AddSub(v, iAct) {
    if (v == 1) {
        if (LesActions[iAct].Periodes.length < 8) {
            LesActions[iAct].Periodes.push({
                Hfin: 2400,
                Type: 1,
                Vmin: 0,
                Vmax: 100,
                Tinf: 1600,
                Tsup: 1600,
                Hmin: 0,
                Hmax: 0,
                CanalTemp: -1,
                SelAct: 255,
                Ooff: 0,
                O_on: 0,
                Tarif: 31
            }); //Tarif codé en bits
            var Hbas = 0;
            if (LesActions[iAct].Periodes.length > 2) {
                Hbas = parseInt(LesActions[iAct].Periodes[LesActions[iAct].Periodes.length - 3].Hfin);
            }
            if (LesActions[iAct].Periodes.length > 1) {
                LesActions[iAct].Periodes[LesActions[iAct].Periodes.length - 2].Hfin = Math.floor((Hbas + 2400) / 2);
            }
        }
    } else {
        if (LesActions[iAct].Periodes.length > 1) {
            LesActions[iAct].Periodes.pop();
            if (LesActions[iAct].Periodes.length > 0)
                LesActions[iAct].Periodes[LesActions[iAct].Periodes.length - 1].Hfin = 2400;
        }
    }
    TracePeriodes(iAct);

}
function infoZclicK(i, iAct) {
    var capteurT = false;
    if (!blockEvent) {
        blockEvent = true;
        var Type = LesActions[iAct].Periodes[i].Type;
        var idZ = "info" + iAct + "Z" + i;
        var S = "<div class='selectZ'> S&eacute;lection Action<div class='closeZ' onclick='infoZclose(\"" + idZ + "\")'>X</div></div>";
        //On ne traite plus depuis version8 le cas "Pas de Contrôle". Inutile
        var check = (Type == 1) ? "checked" : "";
        S += "<div class='zOff'  ><div class='radioC' ><input type='radio'  name='R" + idZ + "' onclick='selectZ(1," + i + "," + iAct + ");' " + check + " title='Off forcé'>OFF</div></div>";
        S += "<div class='fcontainer'><div class='fcontleft'>";
        check = (Type == 2) ? "checked" : "";
        S += "<div  class='zOn'   ><div class='radioC' ><input type='radio'  name='R" + idZ + "' onclick='selectZ(2," + i + "," + iAct + ");' " + check + " 'title='On forcé (si conditions optionnelles valides)'>ON <small>100%</small></div></div>";
        check = (Type > 2) ? "checked" : "";
        var Vmin = LesActions[iAct].Periodes[i].Vmin;
        var Vmax = LesActions[iAct].Periodes[i].Vmax;
        var Tinf = LesActions[iAct].Periodes[i].Tinf;
        var Tsup = LesActions[iAct].Periodes[i].Tsup;
        var TinfC = Tinf / 10;
        var TsupC = Tsup / 10;
        var Hmin = (LesActions[iAct].Periodes[i].Hmin > 0) ? Hdeci2Hmn(LesActions[iAct].Periodes[i].Hmin) : "";
        var Hmax = (LesActions[iAct].Periodes[i].Hmax > 0) ? Hdeci2Hmn(LesActions[iAct].Periodes[i].Hmax) : "";
        var Ooff = (LesActions[iAct].Periodes[i].Ooff > 0) ? LesActions[iAct].Periodes[i].Ooff : "";
        var O_on = (LesActions[iAct].Periodes[i].O_on > 0) ? LesActions[iAct].Periodes[i].O_on : "";
        if (Tinf > 1500 || Tinf < -500) TinfC = ""; //Temperature entre -50 et 150° représenté en dixième
        if (Tsup > 1500 || Tsup < -500) TsupC = ""; //Temperature entre -50 et 150
        if (iAct > 0) {
            var Routage = ["", "Routage ON/Off", "Routage Multi-sinus", "Routage Train de Sinus", "PWM", "Routage Demi-Sinus"];
            S += "<div class='zPw' ><div class='radioC' ><input type='radio'  name='R" + idZ + "' onclick='selectZ(3," + i + "," + iAct + ");' " + check + ">" + Routage[LesActions[iAct].Actif] + "</div>";
            if (LesActions[iAct].Actif <= 1) {
                S += "<div><small>On : &nbsp;</small>Pw &lt;<input id='Pw_min_" + idZ + "'  type='number' value='" + Vmin + "' onchange='NewVal(this)' title='Seuil de puissance pour activer ou désactiver le routage. Attention, en cas de mode On/Off la diff&eacute;rence, seuil sup&eacute;rieur moins  seuil inf&eacute;rieur doit &ecirc;tre sup&eacute;rieure &agrave; la consommation du dipositif pour &eacute;viter l&apos;oscillation du relais de commande.'>W</div>";
                S += "<div><small>Off : </small>Pw &gt;<input id='Pw_max_" + idZ + "'  type='number' value='" + Vmax + "' onchange='NewVal(this)'>W</div>";
                S += "<div><small>Puissance active en entrée de maison</small></div></div>";
            } else {
                S += "<div><small>Seuil Pw : &nbsp;</small><input id='Pw_min_" + idZ + "'  type='number' value='" + Vmin + "' onchange='NewVal(this)' title='Seuil de puissance pour activer ou désactiver le routage.' >W</div>";
                S += "<div><small>Puissance active en entrée de maison</small></div>";
                S += "<div><small>Ouvre Max : </small><input id='Pw_max_" + idZ + "'   type='number' value='" + Vmax + "' onchange='NewVal(this) title='Ouverture maximum du  SSR. Valeur typique : 100%'>%</div></div>";
            }

        } else {
            var Routage = ["", "Routage Découpe Sinus", "Routage Multi-sinus", "Routage Train de Sinus", "", "Routage Demi-Sinus"];
            S += "<div  class='zTriac' ><div class='radioC' ><input type='radio'  name='R" + idZ + "' onclick='selectZ(4," + i + "," + iAct + ");' " + check + ">" + Routage[LesActions[iAct].Actif] + "</div>";
            S += "<div>Seuil Pw &nbsp;<input id='Pw_min_" + idZ + "'  type='number' value='" + Vmin + "' onchange='NewVal(this)' title='Seuil en W de r&eacute;gulation par le Triac de la puissance mesur&eacute;e Pw en entrée de la maison. Valeur typique : 0.'>W</div>";
            S += "<div><small>Puissance active en entrée de maison</small></div>";
            S += "<div>Ouvre Max <input id='Pw_max_" + idZ + "' type='number' value='" + Vmax + "' onchange='NewVal(this)' title='Ouverture maximum du triac. Valeur typique : 100%'>%</div></div>";
        }
        S += "</div>";
        var SelectT = "<div>Canal de Température <select id='CanalTemp" + idZ + "'  onchange='NewVal(this)'><option value=-1 selected>Non exploité</option>";
        for (var c = 0; c < 4; c++) {
            if (LesTemperatures[c] > -100) {
                var Temper = parseFloat(LesTemperatures[c]).toFixed(1);
                SelectT += "<option value=" + c + " >" + NomTemperatures[c] + " (" + Temper + "°)" + "</option>";
                capteurT = true;
            }
        }
        SelectT += "</select></div>";
        var style = (ModePara == 0) ? "none" : "block";
        style = "style='display:" + style + "';";
        S += "<div>";
        S += "<div class='TitZone' " + style + ">&nbsp;&nbsp;&nbsp;Conditions optionnelles pour activer</div>";
        if (capteurT) {
            S += "<div  class='bord1px' " + style + ">";
            S += SelectT;
            S += "<div class='minmax'><div>T &ge;<input id='T_sup_" + idZ + "'  type='number' value='" + TsupC + "' onchange='NewVal(this)' title='Définir la ou les températures qui permettent l&apos;activation de la fonction On ou Routage.'>°</div>";
            S += "<div>T &le;<input id='T_inf_" + idZ + "'  type='number' value='" + TinfC + "' onchange='NewVal(this)' >°</div></div>";
            S += "<div><small>T en degré (-50.0 à 150.0) ou laisser vide</small></div>";
            S += "</div>";
        }
        S += "<div  class='bord1px' " + style + " >";
        S += "<div>Etat d'une Action <select id='SelAct" + idZ + "' onchange='NewVal(this)' >" + SelectActions + "</select></div>";
        S += "<div class='minmax'><div>Durée : </div><div>H &ge;<input id='H_min_" + idZ + "'  type='text' value='" + Hmin + "' onchange='NewVal(this)' >h:mn</div>";
        S += "<div>H &le;<input id='H_max_" + idZ + "'  type='text' value='" + Hmax + "' onchange='NewVal(this)' >h:mn</div></div>";
        S += "<div class='minmax'><div>Seuil : </div><div>On à Off si &le;<input id='O_min_" + idZ + "'  type='number' value='" + Ooff + "' onchange='NewVal(this)' >%</div>";
        S += "<div>Off à On si &ge;<input id='O_max_" + idZ + "'  type='number' value='" + O_on + "' onchange='NewVal(this)' >%</div></div>";
        S += "<div><small>h:mn ou % ou laisser vide</small></div>";
        S += "</div>";


        if (LTARFbin > 0) {
            S += "<div  class='bord1px' >";
            S += "<div title='Condition d&apos;activation suivant la tarification. Sinon ordre Off envoyé ou Triac/SSR se ferme.'>Actif si tarif :</div>";
            if (LTARFbin <= 3) {
                S += "<div id='PleineCreuse'><span style='color:red;'>Heure Pleine</span><input type='checkbox' checked id='TarifPl_" + idZ + "' onchange='NewVal(this)'> <span style='color:green;'>Heure Creuse</span><input type='checkbox' checked id='TarifCr_" + idZ + "' onchange='NewVal(this)'></div>";
            } else {
                S += "<div id='Tempo'>Tempo <span style='color:blue;'>Bleu</span><input type='checkbox' checked id='TarifBe_" + idZ + "' onchange='NewVal(this)'><span style='color:white;'> Blanc</span><input type='checkbox' checked id='TarifBa_" + idZ + "' onchange='NewVal(this)'><span style='color:red;'> Rouge</span><input type='checkbox' checked id='TarifRo_" + idZ + "' onchange='NewVal(this)'></div>";
            }
            S += "</div>";
        }
        S += "</div>";

        S += "</div>";
        GH(idZ, S);
        if (capteurT) GID("CanalTemp" + idZ).value = LesActions[iAct].Periodes[i].CanalTemp;
        GID("SelAct" + idZ).value = LesActions[iAct].Periodes[i].SelAct;
        var Tarif_ = LesActions[iAct].Periodes[i].Tarif;
        if (LTARFbin > 0) {
            if (LTARFbin <= 3) {
                GID("TarifPl_" + idZ).checked = (Tarif_ & 1) ? 1 : 0; // H Pleine
                GID("TarifCr_" + idZ).checked = (Tarif_ & 2) ? 1 : 0;
            } else {
                GID("TarifBe_" + idZ).checked = (Tarif_ & 4) ? 1 : 0;
                GID("TarifBa_" + idZ).checked = (Tarif_ & 8) ? 1 : 0;
                GID("TarifRo_" + idZ).checked = (Tarif_ & 16) ? 1 : 0; //Rouge
            }
        }
        GID(idZ).style.display = "block";
    }
}

)====";

const char * ActionsJS3 = R"====(
function infoZclose(idx) {
    var champs = idx.split("info");
    var idx = champs[1].split("Z");
    S = "TracePeriodes(" + idx[0] + ");";
    setTimeout(S, 100);
}
function selectZ(T, i, iAct) {
    if (LesActions[iAct].Periodes[i].Type != T) {
        LesActions[iAct].Periodes[i].Type = T;
        var idZ = "info" + iAct + "Z" + i;
        if (T <= 1) {
            infoZclose(idZ);
            TracePeriodes(iAct);
        }
    }
}

function NewVal(t) {
    var champs = t.id.split("info");
    var idx = champs[1].split("Z");   //Num Action, Num période
    if (champs[0].indexOf("Pw_min") >= 0) {
        LesActions[idx[0]].Periodes[idx[1]].Vmin = Math.floor(GID(t.id).value);
    }
    if (champs[0].indexOf("Pw_max") >= 0) {
        LesActions[idx[0]].Periodes[idx[1]].Vmax = Math.floor(GID(t.id).value);
        if (idx[0] == 0) {
            LesActions[idx[0]].Periodes[idx[1]].Vmax = Math.max(LesActions[idx[0]].Periodes[idx[1]].Vmax, 5);
            LesActions[idx[0]].Periodes[idx[1]].Vmax = Math.min(LesActions[idx[0]].Periodes[idx[1]].Vmax, 100);
        }
    }
    if (champs[0].indexOf("inf") > 0) {
        var V = GID(t.id).value;
        if (V == "") V = 158;
        LesActions[idx[0]].Periodes[idx[1]].Tinf = Math.floor(V * 10);
    }
    if (champs[0].indexOf("sup") > 0) {
        var V = GID(t.id).value;
        if (V == "") V = 158;
        LesActions[idx[0]].Periodes[idx[1]].Tsup = Math.floor(V * 10);
    }
    if (champs[0].indexOf("H_min") >= 0) {
        LesActions[idx[0]].Periodes[idx[1]].Hmin = Hmn2Hdeci(GID(t.id).value);
    }
    if (champs[0].indexOf("H_max") >= 0) {
        LesActions[idx[0]].Periodes[idx[1]].Hmax = Hmn2Hdeci(GID(t.id).value);
    }
    if (champs[0].indexOf("O_min") >= 0) {
        LesActions[idx[0]].Periodes[idx[1]].Ooff = Math.max(0, Math.min(100, Math.floor(GID(t.id).value)));
    }
    if (champs[0].indexOf("O_max") >= 0) {
        LesActions[idx[0]].Periodes[idx[1]].O_on = Math.max(0, Math.min(100, Math.floor(GID(t.id).value)));
    }
    if (champs[0].indexOf("Tarif") >= 0) {
        var idZ = "info" + champs[1];
        var Tarif_ = 0;
        if (LTARFbin <= 3) {
            Tarif_ += GID("TarifPl_" + idZ).checked ? 1 : 0; //H pleine
            Tarif_ += GID("TarifCr_" + idZ).checked ? 2 : 0;
        } else {
            Tarif_ += GID("TarifBe_" + idZ).checked ? 4 : 0; //Bleu
            Tarif_ += GID("TarifBa_" + idZ).checked ? 8 : 0;
            Tarif_ += GID("TarifRo_" + idZ).checked ? 16 : 0; //Rouge
        }
        LesActions[idx[0]].Periodes[idx[1]].Tarif = Tarif_;
    }
    if (champs[0].indexOf("CanalTemp") >= 0) {
        LesActions[idx[0]].Periodes[idx[1]].CanalTemp = GID(t.id).value;
    }
    if (champs[0].indexOf("SelAct") >= 0) {
        LesActions[idx[0]].Periodes[idx[1]].SelAct = GID(t.id).value;
    }
}


function editTitre(iAct) {
    if (GID("titre" + iAct).innerHTML.indexOf("<input") == -1) {
        GH("titre" + iAct, "<input type='text' value='" + GID("titre" + iAct).innerHTML + "' id='Etitre" + iAct + "'  onblur='TitreValid(" + iAct + ")' >");
        GID("Etitre" + iAct).focus();
    }
}
function TitreValid(iAct) {
    LesActions[iAct].Titre = GID("Etitre" + iAct).value.trim();
    GH("titre" + iAct, LesActions[iAct].Titre);
}
function checkDisabled() {
    GID("sortie0").style.display = "none";
    GID("Freq_PWM").style.display = "none";
    GID("commun").style.display = (ModePara > 0 && ReacCACSI < 100) ? "block" : "none";
    for (var iAct = 0; iAct < LesActions.length; iAct++) {
        for (var i = 0; i <= 5; i++) {
            if (GID("radio" + iAct + "-" + i).checked) { LesActions[iAct].Actif = i; } //0=Inactif,1=Decoupe ou On/Off, 2=Multi, 3= Train, 4= PWM, 5=Demi-Sinus
        }
        if (GID("selectPin" + iAct).value == -1 && LesActions[iAct].Actif > 1 && iAct > 0) { LesActions[iAct].Actif = 1; GID("radio" + iAct + "-" + LesActions[iAct].Actif).checked = true; }
        TracePeriodes(iAct);
        GID("planning0").style.display = (pTriac > 0) ? "block" : "none";  // Si Pas de Triac 
        GID("TitrTriac").style.display = (pTriac > 0) ? "block" : "none";
        GID("blocPlanning" + iAct).style.display = (LesActions[iAct].Actif > 0) ? "block" : "none";
        GID("visu" + iAct).style.display = (LesActions[iAct].Actif > 0) ? "block" : "none";
        if (LesActions[iAct].Actif == 1 && iAct>0){
             GID('graphAction' + iAct).style.display ="none";
             GID("visu" + iAct).style.display ="none";
        }
        var visible = (LesActions[iAct].Actif == 1) ? "block" : "none";
        GID("Tempo" + iAct).style.display = visible;
        var disable = true;
        var disp = "block";
        if (GID("selectPin" + iAct).value >= 0) { visible = "none"; disable = false; disp = "none"; }
        GID("SelectOut" + iAct).style.display = (GID("selectPin" + iAct).value <= 0) ? "none" : "inline-block";
        GID("Host" + iAct).style.display = visible;
        GID("Port" + iAct).style.display = visible;
        GID("Repet" + iAct).style.display = visible;
        GID("radio" + iAct + "-2").disabled = disable;
        GID("radio" + iAct + "-3").disabled = disable;
        GID("radio" + iAct + "-4").disabled = disable;
        GID("radio" + iAct + "-5").disabled = disable;
        GID("ordreoff" + iAct).style.display = disp;
        GID("ordreon" + iAct).style.display = disp;
        if (GID("selectPin" + iAct).value == -1 && GID("ordreOn" + iAct).value.indexOf(IS) > 0) GID("ordreOn" + iAct).value = "";
        GID("ligne_bas" + iAct).style.display = (LesActions[iAct].Actif == 1 && GID("selectPin" + iAct).value <= 0 && iAct > 0) ? "flex" : "none";
        GID("fen_slide" + iAct).style.display = (LesActions[iAct].Actif == 1 && iAct > 0) ? "none" : "table";
        if (GID("radio" + iAct + "-4").checked) {
            GID("Freq_PWM").style.display = "block";
            GID("commun").style.display = "block";
        }
        if (ModePara == 0) {
            GID("PID" + iAct).checked = false;
        }
        GID("PIDbox" + iAct).style.display = (ModePara == 0 || (GID("selectPin" + iAct).value <= 0 && iAct > 0) || (LesActions[iAct].Actif == 1 && iAct > 0)) ? "none" : "block";
        GID("Propor" + iAct).style.display = (GID("PID" + iAct).checked) ? "table-row" : "none";
        GID("Derive" + iAct).style.display = (GID("PID" + iAct).checked) ? "table-row" : "none";

    }
    GID("Pwm0").style.display = "none"; //Pas de PWM sur la sortie Triac
}

function LoadActions() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var LeRetour = this.responseText;
            var Les_ACTIONS = LeRetour.split(GS);
            var LesParas = Les_ACTIONS[0].split(RS);
            LesTemperatures = LesParas[0].split("|");
            NomTemperatures = LesParas[1].split(US);
            LTARFbin = parseInt(LesParas[2]);
            pTriac = parseInt(LesParas[3]);
            ReacCACSI = LesParas[4]; //1,24 8 ou 100 pour EstimCACSI
            if (ReacCACSI < 100) {
                GID("CACSI" + ReacCACSI).checked = true; //Reactivité Ki CACSI et non Estimation
                GID("CACSI").style = "display:block;";
            }
            GID("Fpwm" + LesParas[5]).checked = true;
            LesActions.splice(0, LesActions.length);
            for (var iAct = 1; iAct < Les_ACTIONS.length - 1; iAct++) {
                var champs = Les_ACTIONS[iAct].split(RS);
                var NbPeriodes = champs[12];
                var Periodes = [];
                var j = 13;
                for (var i = 0; i < NbPeriodes; i++) {
                    Periodes[i] = { Type: champs[j], Hfin: champs[j + 1], Vmin: champs[j + 2], Vmax: champs[j + 3], Tinf: champs[j + 4], Tsup: champs[j + 5], Hmin: champs[j + 6], Hmax: champs[j + 7], CanalTemp: champs[j + 8], SelAct: champs[j + 9], Ooff: champs[j + 10], O_on: champs[j + 11], Tarif: champs[j + 12] };
                    j = j + 13;
                }
                LesActions[iAct - 1] = creerAction(champs[0], champs[1], champs[2], champs[3], champs[4], champs[5], champs[6], champs[7], champs[8], champs[9], champs[10], champs[11], Periodes);
            }
            if (LesActions.length == 0) {  //Action Triac
                LesActions.push(creerAction(0, "Titre Triac", "", 50, "", "", "", 0, 10, 10, 10, 0, [{
                    Hfin: 2400,
                    Type: 4,
                    Vmin: 0,
                    Vmax: 100,
                    Tinf: 1600,
                    Tsup: 1600,
                    Hmin: 0,
                    Hmax: 0,
                    CanalTemp: -1,
                    SelAct: 255,
                    Ooff: 0,
                    O_on: 0,
                    Tarif: 31
                }
                ]));
            }
            LesActions.push(creerAction(0, "Titre Relais " + LesActions.length, "", 80, "", "", 240, 0, 10, 10, 10, 0, [{
                Hfin: 2400,
                Type: 3,
                Vmin: 0,
                Vmax: 100,
                Tinf: 1600,
                Tsup: 1600,
                Hmin: 0,
                Hmax: 0,
                CanalTemp: -1,
                SelAct: 255,
                Ooff: 0,
                O_on: 0,
                Tarif: 31
            }
            ]));
            var S = "";
            for (var i = 1; i < LesActions.length; i++) {
                S += "<div id='planning" + i + "' class='planning' ></div>";
            }
            let imax = LesActions.length - 1;

            S += "<input id='butR' type='button'  class='tbut' value='+' onclick='this.style.display=\"none\";GID(\"planning" + imax + "\").style.display=\"block\";' title='Rajouter un relais d&apos;action.'>";
            GH("plannings", S);
            GID("planning" + imax).style.display = "none";
            for (var iAct = 0; iAct < LesActions.length; iAct++) {
                TracePlanning(iAct);
            }
            checkDisabled();
            LoadParaRouteur();
        }
    };
    xhttp.open('GET', '/ActionsAjax', true);
    xhttp.send();
}
function UpdateK(iAct) {
    if (PlotIdx >= 0) { //envoi direct des coef si on visualise en temps réel la régulation. Ils ne sont pas enregistrés.
        let Kp = Math.floor(GID("sliderKp" + iAct).value);
        let Ki = Math.floor(GID("sliderKi" + iAct).value);
        let Kd = Math.floor(GID("sliderKd" + iAct).value);

        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                var retour = this.responseText;
            }
        };
        xhttp.open('GET', '/UpdateK?iAct=' + iAct + '&Kp=' + Kp + '&Ki=' + Ki + '&Kd=' + Kd, true);
        xhttp.send();
    }

}
)====";

const char * ActionsJS4 = R"====(
function SendValues() {
    GID("attente").style = "visibility: visible;";
    for (var iAct = 0; iAct < LesActions.length; iAct++) {
        for (var i = 0; i <= 4; i++) {
            if (GID("radio" + iAct + "-" + i).checked) { LesActions[iAct].Actif = i; }
        }
        LesActions[iAct].Titre = GID("titre" + iAct).innerHTML.trim();
        LesActions[iAct].Host = GID("host" + iAct).value.trim();
        LesActions[iAct].Port = GID("port" + iAct).value;
        LesActions[iAct].OrdreOn = GID("ordreOn" + iAct).value.trim();
        LesActions[iAct].OrdreOff = GID("ordreOff" + iAct).value.trim();
        LesActions[iAct].Repet = GID("repet" + iAct).value;
        LesActions[iAct].Tempo = GID("tempo" + iAct).value;
        LesActions[iAct].Kp = GID("sliderKp" + iAct).value;
        LesActions[iAct].Ki = GID("sliderKi" + iAct).value;
        LesActions[iAct].Kd = GID("sliderKd" + iAct).value;
        LesActions[iAct].PID = GID("PID" + iAct).checked ? 1 : 0;
        if (GID("selectPin" + iAct).value >= 0) LesActions[iAct].OrdreOn = GID("selectPin" + iAct).value + IS + GID("selectOut" + iAct).value;
        if (iAct > 0 && (GID("selectPin" + iAct).value == 0 || LesActions[iAct].Titre == "")) LesActions[iAct].Actif = -1; //Action à effacer
    }
    var S = "";
    for (var iAct = 0; iAct < LesActions.length; iAct++) {
        if (LesActions[iAct].Actif >= 0) {
            S += LesActions[iAct].Actif + RS + LesActions[iAct].Titre + RS;
            S += LesActions[iAct].Host + RS + LesActions[iAct].Port + RS;
            S += LesActions[iAct].OrdreOn + RS + LesActions[iAct].OrdreOff + RS + LesActions[iAct].Repet + RS + LesActions[iAct].Tempo + RS;
            S += LesActions[iAct].Kp + RS + LesActions[iAct].Ki + RS + LesActions[iAct].Kd + RS + LesActions[iAct].PID + RS + LesActions[iAct].Periodes.length + RS;
            for (var i = 0; i < LesActions[iAct].Periodes.length; i++) {
                if (ModePara == 0) { //Standard
                    LesActions[iAct].Periodes[i].CanalTemp = -1;
                    LesActions[iAct].Periodes[i].SelAct = 255;
                }
                S += LesActions[iAct].Periodes[i].Type + RS + Math.floor(LesActions[iAct].Periodes[i].Hfin) + RS;
                S += Math.floor(LesActions[iAct].Periodes[i].Vmin) + RS + Math.floor(LesActions[iAct].Periodes[i].Vmax) + RS;
                S += Math.floor(LesActions[iAct].Periodes[i].Tinf) + RS + Math.floor(LesActions[iAct].Periodes[i].Tsup) + RS;
                S += Math.floor(LesActions[iAct].Periodes[i].Hmin) + RS + Math.floor(LesActions[iAct].Periodes[i].Hmax) + RS;
                S += Math.floor(LesActions[iAct].Periodes[i].CanalTemp) + RS + Math.floor(LesActions[iAct].Periodes[i].SelAct) + RS;
                S += Math.floor(LesActions[iAct].Periodes[i].Ooff) + RS + Math.floor(LesActions[iAct].Periodes[i].O_on) + RS;
                S += LesActions[iAct].Periodes[i].Tarif + RS;
            }
            S += GS;
        }
    }
    if (ReacCACSI < 100) ReacCACSI = document.querySelector('input[name="ReacCACSI"]:checked').value; //Pas d'estimation
    var Fpwm = document.querySelector('input[name="Fpwm"]:checked').value;
    S = encodeURIComponent(S);
    S = "?ReacCACSI=" + ReacCACSI + "&Fpwm=" + Fpwm + "&actions=" + S + "|"; //On ne peut pas terminer par GS

    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var retour = this.responseText;
            location.reload();
        }
    };
    xhttp.open('GET', '/ActionsUpdate' + S, true);
    xhttp.send();

}
function Plot(iAct) {
    for (i = 0; i < LesActions.length; i++) {
        GID('graphAction' + i).style.display = "none";
    }
    if (PlotIdx != iAct) {
        PlotIdx = iAct;
        GID('graphAction' + iAct).style.display = "block";
        PlotR(iAct);
    } else {
        PlotIdx = -1;
    }


}
function PlotR(iAct) { // Courbes détaillées PID
    if (PlotIdx >= 0) {
        if (!Pause) {
            TabIdx = (TabIdx + 1) % 150;
            Ecart[TabIdx] = parseInt(MesuresPwAct[0]);
            Prop[TabIdx] = parseFloat(MesuresPwAct[1]);
            Integ[TabIdx] = parseFloat(MesuresPwAct[2]);
            Deriv[TabIdx] = parseFloat(MesuresPwAct[3]);
            let retard = Prop[TabIdx] + Integ[TabIdx] + Deriv[TabIdx];
            Retard[TabIdx] = Math.floor(Math.max(Math.min(100, retard), 0));
            Ouvert[TabIdx] = 100 - Retard[TabIdx];
            var cW = "#" + Koul[Coul_W][3];
            var cOuvre = "#" + Koul[Coul_Ouvre][3];
            var cRetard = "#" + Koul[Coul_VA][3];
            var cT = "#" + Koul[Coul_Graphe][1];
            var cP = "#" + Koul[Coul_Temp][3];
            var cI = "#" + Koul[Coul_Temp + 1][3];
            var cD = "#" + Koul[Coul_Temp + 3][3];
            var style = 'background:linear-gradient(#' + Koul[Coul_Graphe][5] + ',#' + Koul[Coul_Graphe][3] + ',#' + Koul[Coul_Graphe][5] + ');border-color:#' + Koul[Coul_Tab][5] + ';';
            var S = "<svg viewbox='0 0 970 240' style='" + style + "' height='240'  >"; //   
            S += "<line x1='50' y1='20' x2='50' y2='220' style='stroke:" + cW + ";stroke-width:2' />";
            S += "<line x1='800' y1='20' x2='800' y2='220' style='stroke:" + cOuvre + ";stroke-width:2' />";
            S += "<line x1='50' y1='220' x2='800' y2='220' style='stroke:" + cT + ";stroke-width:2' />";
            for (var t = 0; t < 75; t = t + 5) {
                let x = 800 - t * 10;
                S += "<text x='" + x + "' y='237' style='font-size:16px;fill:" + cT + ";'>" + t + "</text>";
                S += "<line x1='" + x + "' y1='220' x2='" + x + "' y2='224' style='stroke:" + cT + ";stroke-width:2' />";
            }
            S += "<text x='790' y='218' style='font-size:12px;fill:" + cT + ";'>s</text>";
            S += "<text x='804' y='220' style='font-size:12px;fill:" + cOuvre + ";'>0%</text>";
            S += "<text x='804' y='20' style='font-size:12px;fill:" + cOuvre + ";'>100%</text>";
            S += "<line x1='50' y1='20' x2='800' y2='20' style='stroke:" + cOuvre + ";stroke-width:2' stroke-dasharray='1 4' />";
            S += "<line x1='50' y1='120' x2='800' y2='120' style='stroke:" + cW + ";stroke-width:2' stroke-dasharray='1 4' />";
            S += "<text x='20' y='120' style='font-size:12px;fill:" + cW + ";'>0 W</text>";
            S += "<text x='5' y='20' style='font-size:12px;fill:" + cW + ";'>100 W</text>";
            S += "<text x='1' y='220' style='font-size:12px;fill:" + cW + ";'>-100 W</text>";


            S += "<text x='830' y='40' style='font-size:16px;fill:" + cW + ";'>Ecart..........." + Ecart[TabIdx] + " W</text>";
            S += "<text x='830' y='60' style='font-size:16px;fill:" + cOuvre + ";'>Ouverture....." + Ouvert[TabIdx] + " %</text>";
            S += "<text x='860' y='76' style='font-size:12px;fill:" + cT + ";'>= 100 - Retard</text>";
            S += "<text x='830' y='96' style='font-size:16px;fill:" + cRetard + ";'>Retard........." + Retard[TabIdx] + " %</text>";            
            S += "<text x='830' y='168' style='font-size:16px;fill:" + cI + ";'>Intégral....." + Integ[TabIdx] + "</text>";
            
            if (ModePara == 1 && GID("PID" + iAct).checked ){
                S += "<text x='860' y='112' style='font-size:12px;fill:" + cT + ";'>=</text>";
                S += "<text x='830' y='132' style='font-size:16px;fill:" + cP + ";'>Proportion.." + Prop[TabIdx] + "</text>";
                S += "<text x='860' y='148' style='font-size:12px;fill:" + cT + ";'>+</text>";
                S += "<text x='860' y='184' style='font-size:12px;fill:" + cT + ";'>+</text>";
                S += "<text x='830' y='204' style='font-size:16px;fill:" + cD + ";'>Dérivée........" + Deriv[TabIdx] + "</text>";
            }

            let SE = "<polyline points='";
            let SO = "<polyline points='";
            let SR = "<polyline points='";
            let SP = "<polyline points='";
            let SI = "<polyline points='";
            let SD = "<polyline points='";
            for (let i = 0; i < 150; i++) { //toutes les 0.5s
                let j = (TabIdx - i + 150) % 150;
                let x = 800 - i * 5;
                let y = 120 - Ecart[j];
                y = Math.min(y, 220); y = Math.max(y, 20);
                SE += x + "," + y + " ";
                y = 220 - 2 * Ouvert[j];
                SO += x + "," + y + " ";
                y = 220 - 2 * Retard[j];
                SR += x + "," + y + " ";
                y = 120 - Prop[j];
                y = Math.min(y, 220); y = Math.max(y, 20);
                SP += x + "," + y + " ";
                y = 220 - 2 * Integ[j];
                SI += x + "," + y + " ";
                y = 120 - Deriv[j];
                y = Math.min(y, 220); y = Math.max(y, 20);
                SD += x + "," + y + " ";
            }
            SE += "' style='fill:none;stroke:" + cW + ";stroke-width:2' />";
            SO += "' style='fill:none;stroke:" + cOuvre + ";stroke-width:2' />";
            SR += "' style='fill:none;stroke:" + cRetard + ";stroke-width:2' />";
            SP += "' style='fill:none;stroke:" + cP + ";stroke-width:2' />";
            SI += "' style='fill:none;stroke:" + cI + ";stroke-width:2' />";
            SD += "' style='fill:none;stroke:" + cD + ";stroke-width:2' />";
             if (ModePara == 1 && GID("PID" + iAct).checked){
                    S += SD  + SP;
             }
            S +=  SI  + SR + SO + SE + "</svg>";
            GID('graphSVG' + iAct).innerHTML = S;

        }
        let P = "PlotR(" + iAct + ");"
        setTimeout(P, 500);
    }
}

function ShowAction() {
    let Dt = 200; //ms
    if (PlotIdx >= 0) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                var retour = this.responseText;
                MesuresPwAct.length = 0;
                MesuresPwAct = retour.split(RS);

            }
        };
        xhttp.open('GET', '/ShowAction?NumAction=' + PlotIdx, true);
        xhttp.send();
    } else {
        for (var i = 0; i < 4; i++) {
            MesuresPwAct[i] = 0;
        }
        for (var i = 0; i < 150; i++) {
            Ecart[i] = 0;
            Ouvert[i] = 0;
            Retard[i] = 0;
            Prop[i] = 0;
            Integ[i] = 0;
            Deriv[i] = 0;
        }
        Dt = 2000;
    }
    setTimeout("ShowAction();", Dt);
}

function FinParaRouteur() {
    SelectActions = "<option value=255>Non exploité</option>";
    for (esp = 0; esp < nomRMS.length; esp++) { //Liste des actions par routeur
        for (var iAct = 0; iAct < nomActions[esp].length; iAct++) {
            var v = esp * 10 + parseInt(nomActions[esp][iAct][0]); //Nombre refletant la référence esp et action
            var T = (esp == 0) ? "" : nomRMS[esp] + " / ";
            SelectActions += "<option value=" + v + ">" + T + nomActions[esp][iAct][1] + "</option>";
            ListeActions[v] = T + nomActions[esp][iAct][1];
        }
    }
    for (var iAct = 0; iAct < LesActions.length; iAct++) {
        GID("PID" + iAct).checked = LesActions[iAct].PID == 1 ? true : false;
    }
    checkDisabled();
}



function AdaptationSource() {

}
)====";