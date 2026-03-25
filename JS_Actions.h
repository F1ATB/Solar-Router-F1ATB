const char * ActionsJS1 = R"====(
// Structure de données 
var mouseClick = false;
var blockEvent = false;
var draggedHandle = null; 
var ListeActions = [];
var SelectActions = "";
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
var IS = "|"; // Input Separator
var BordsInverse = [".Bactions"];

/**
 * Fonction d'initialisation appelée au chargement.
 */
function Init() {
    SetHautBas();
    LoadParaFixe();   
}

/**
 * Crée un objet représentant une action.
 */

function CreerAction(NumAction, Titre) {
    let type=3; //Pw pour les relais
    if (NumAction==0) type=4; //Pw pour le Triac
    const S = {
        Action: NumAction,
        Actif: 0,
        Titre: Titre,
        Host: "",
        Port: 80,
        OrdreOn: "",
        OrdreOff: "",
        Repet: 240,
        Tempo: 0,
        Kp: 10,
        Ki: 10,
        Kd: 10,
        PID: 0,
        ForceOuvre: 100,
        IdxSequenceur: -1,
        PuissanceCharge: 0,
        Periodes: [{
                    Hfin: 2400,
                    Type: type,
                    Vmin: 0,
                    Vmax: 100,
                    ONouvre: 100,
                    Tinf: 1600,
                    Tsup: 1600,
                    Hmin: 0,
                    Hmax: 0,
                    CanalTemp: -1,
                    SelAct: 255,
                    Ooff: 0,
                    O_on: 0,
                    Tarif: 31
                }]
    };
    return S;
}

/**
 * Trace le panneau de configuration pour une action donnée.
 * @param {number} iAct - Index de l'action.
 */
function TracePlanning(iAct) {
    // Modes d'activation
    let Radio0 = "<div><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-0' onclick='checkDisabled();'>Inactif</div>";
    let Radio1 = "<div><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-1' onclick='checkDisabled();'>Découpe sinus</div>";
    if (iAct > 0) { Radio1 = "<div><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-1' onclick='checkDisabled();'>On/Off</div>"; }
    Radio1 += "<div><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-5' onclick='checkDisabled();'>Demi-sinus</div>";
    Radio1 += "<div><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-2' onclick='checkDisabled();'>Multi-sinus</div>";
    Radio1 += "<div><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-3' onclick='checkDisabled();'>Train de sinus</div>";
    Radio1 += "<div id='Pwm" + iAct + "'><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-4' onclick='checkDisabled();'>PWM</div>";
    Radio1 += "<div id='Sequenceur" + iAct + "'><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-6' onclick='checkDisabled();' title='Distribue la puissance séquentiellement sur plusieurs charges pour limiter les harmoniques'>Séquenceur de relais</div>";

    // Sélecteur de pin GPIO
    let SelectPin = "<div id='SelectPin" + iAct + "'>Gpio <select id='selectPin" + iAct + "' onchange='checkDisabled();' title='Choix broche (GPIO) de commande'>";
    for (let i = 0; i < Pins.length; i++) {
        let v = "gpio:" + Pins[i];
        if (Pins[i] == 0) v = "pas choisi ou supprimer";
        if (Pins[i] == -1) v = "Externe";
        SelectPin += "<option value=" + Pins[i]  + ">" + v + "</option>";
    }
    SelectPin += "</select></div>";
    
    // Sélecteur de sortie (niveau logique)
    let SelectOut = "<div id='SelectOut" + iAct + "'>Sortie 'On' <select id='selectOut" + iAct + "' title='Dépend du relais. En général +3.3V'><option value=0>0V</option><option value=1 selected>3.3V</option></select></div>";

    let S = "<div class='titre'><span id ='titre" + iAct + "' onclick='editTitre(" + iAct + ")' title='Donnez un nom ou effacez pour supprimer l'action'>Titre</span></div>";
    S += "<div class='visu' onclick='Plot(" + iAct + ")' id='visu" + iAct + "' title='Zoom sur la régulation en temps réel. Les réglages peuvent être modifiés. Ne pas oublier de sauvegarder'>&#128200;</div>";
    S += "<div class='mode'><div class='TitZone' title='Choix du mode de découpe du secteur'>Mode</div>" + Radio0 + Radio1 + "</div>";
    S += "<div id='blocPlanning" + iAct + "'>";
    S += "<div class='les_select' id='sortie" + iAct + "'>";
    S += "<div class='TitZone' title='Définir la broche (GPIO) ou commande'>Sortie</div>" + SelectPin + SelectOut;
    S += "<div><span id='Tempo" + iAct + "'>Temporisation(s) <input type='number' class='tm' id='tempo" + iAct + "' title='Temporisation entre chaque changement d&apos;état pour éviter des oscillations quand un appareil dans la maison consomme en dents de scie (Ex: un four).'></span></div>";
    S += "<div><span id='forceOuvre" + iAct + "'>Ouverture si forcée<input type='number' class='tm' id='ForceOuvre" + iAct + "' min='0' max='100' step='1' title='Ouverture prioritaire maximum en % si action forcée'>%</span> </div>";
    S += "</div><div class='les_select' id='ligne_bas" + iAct + "'><div class='TitZone'>Externe</div>";
    S += "<div><span id='Host" + iAct + "'>Host<br><input type='text' id='host" + iAct + "' onchange='checkDisabled();' title='Adresse IP machine sur réseau LAN, nom de domaine ou rien.'></span></div>";
    S += "<div><span id='Port" + iAct + "'>Port<br><input type='number' class='tm' id='port" + iAct + "' title='Port d&apos;acc&egrave;s via le protocole http , uniquement pour machine distante. En g&eacute;n&eacute;ral <b>80</b>.'></span></div>";
    S += "<div><span id='ordreon" + iAct + "'>Ordre On<br><input type='text' id='ordreOn" + iAct + "' title='Ordre à passer à la machine distante.'></span></div>";
    S += "<div><span id='ordreoff" + iAct + "'>Ordre Off<br><input type='text' id='ordreOff" + iAct + "' ></span></div>";
    S += "<div><span id='Repet" + iAct + "'>Répétition(s)<br><input type='number' id='repet" + iAct + "' class='tm' title='P&eacute;riode en s de r&eacute;p&eacute;tition/rafra&icirc;chissement de la commande. Uniquement pour les commandes vers l&apos;extérieur. 0= pas de répétition.'></span></div>";
    S += "</div>";

    // Panneau séquenceur (informatif, affiché uniquement si mode=6)
    S += "<div id='groupeSequenceur" + iAct + "' class='les_select' style='display:none;'>";
    S += "<div class='TitZone'>S&eacute;quenceur de relais</div>";
    S += "<div style='padding:4px;color:#aaa;font-size:0.9em;'>Ce mode distribue l'ouverture PID vers les relais gérés. Aucune sortie GPIO directe.</div>";
    S += "</div>";

    // Panneau relais séquencé : sélecteur de séquenceur parent + puissance nominale
    let optionsSequenceur = "<option value='-1'>— aucun —</option>";
    for (let m = 0; m < F.Actions.length; m++) {
        if (m !== iAct && F.Actions[m].Actif === 6) {
            optionsSequenceur += "<option value='" + m + "'>" + (F.Actions[m].Titre || ("Action " + m)) + "</option>";
        }
    }
    S += "<div id='relaisSequence" + iAct + "' class='les_select' style='display:none;'>";
    S += "<div class='TitZone'>Relais s&eacute;quenc&eacute;</div>";
    S += "<div style='display:inline-block'><span>S&eacute;quenceur<br><select id='selectSequenceur" + iAct + "' onchange='checkDisabled();' title='S&eacute;quenceur qui distribue la puissance vers ce relais s&eacute;quenc&eacute;'>" + optionsSequenceur + "</select></span></div>";
    S += "<div style='display:inline-block'><span>Puissance de la charge&nbsp;(W)<br><input type='number' class='tm' id='puissanceCharge" + iAct + "' min='0' max='10000' value='0' title='Puissance nominale de cette charge en W. 0 = 1000 W par d&eacute;faut.'></span></div>";
    S += "</div>";

    // Curseurs PID
    S += "<div id='planningControles" + iAct + "' class='bouton_curseur'><div class='boutons'><input  type='button' value='-' class='tbut' onclick='AddSub(-1," + iAct + ")' title='Retrait d&apos;une p&eacute;riode horaire.'>";
    S += "<input  type='button' value='+' class='tbut' onclick='AddSub(1," + iAct + ")' title='Ajout d&apos;une p&eacute;riode horaire.'></div>";
    S += "<div class='slideTriac' id='fen_slide" + iAct + "'>";
    S += "<div class='slideTriacIn' id='Propor" + iAct + "'>";
    S += "<div class='Tcell1'>Coef. Proportionnel</div>";
    S += "<div class='Tcell2'><input type='range' min='0' max='100' value='50' title='Correction proportionnelle à l&apos;écart en W' id='sliderKp" + iAct + "' style='width:100%;max-width:none;' oninput=\"GH('sensiKp" + iAct + "',Math.floor(this.value));UpdateK(" + iAct + ");\"></div>";
    S += "<div class='Tcell3'><strong id='sensiKp" + iAct + "'></strong></div>";
    S += "</div>";
    S += "<div class='slideTriacIn'>";
    S += "<div class='Tcell1'>Coef. Intégral ou R&eacute;activit&eacute;</div>";
    S += "<div class='Tcell2'><input type='range' min='0' max='100' value='50' title='Correction suivant l&apos;intégrale de l&apos;écart en W' id='sliderKi" + iAct + "' style='width:100%;max-width:none;' oninput=\"GH('sensiKi" + iAct + "',Math.floor(this.value));UpdateK(" + iAct + ");\"></div>";
    S += "<div class='Tcell3'><strong id='sensiKi" + iAct + "'></strong></div>";
    S += "</div>";
    S += "<div class='slideTriacIn' title='Correction suivant la derivée de l&apos;écart en W' id='Derive" + iAct + "'>";
    S += "<div class='Tcell1'>Coef. Dérivé</div>";
    S += "<div class='Tcell2'><input type='range' min='0' max='100' value='50' id='sliderKd" + iAct + "' style='width:100%;max-width:none;' oninput=\"GH('sensiKd" + iAct + "',Math.floor(this.value));UpdateK(" + iAct + ");\"></div>";
    S += "<div class='Tcell3'><strong id='sensiKd" + iAct + "'></strong></div>";
    S += "</div>";
    S += "</div>";
    S += "<div id='PIDbox" + iAct + "'><label for='PID" + iAct + "'> PID On</label><input type='checkbox' id='PID" + iAct + "' onclick='checkDisabled();'></div>";
    S += "</div>";
    
   
    S += "<div id='graphAction" + iAct + "' class='graphAction'><div id='graphSVG" + iAct + "' class='graphSVG'></div><div class='GraphSVG' onclick='Pause=!Pause;'>&#x23EF;</div></div>";
    
    // Conteneur des curseurs de période (avec correction onmouseup)
    S += "<div style='margin:4px;' id='planningPeriodes" + iAct + "'>";
    S += "<div id='infoAction" + iAct + "' class='infoAction'></div>";
    S += "<div id='curseurs" + iAct + "' class='curseur' onmousemove='dragHandle(event," + iAct + ");' onmouseup='stopDrag();' ontouchmove='dragHandle(event," + iAct + ");' ontouchend='stopDrag();'></div>";
    S += "</div>";
    S += "</div>"; // Ferme le bloc principal

    // Remplissage des valeurs depuis F.Actions
    GH("planning" + iAct, S);
    const action = F.Actions[iAct];
    
    if (action) {
        // Mode Actif
        const radioActive = GID("radio" + iAct + "-" + action.Actif);
        if (radioActive) radioActive.checked = true;
        action.Action=iAct ;
        if (iAct==0 && action.Titre =="") action.Titre="Triac";
        GH("titre" + iAct, action.Titre);
        GV("host" + iAct, action.Host);
        GV("port" + iAct, action.Port);
        GV("ordreOn" + iAct, action.OrdreOn);
        GV("ordreOff" + iAct, action.OrdreOff);
        GV("repet" + iAct, action.Repet);
        GV("tempo" + iAct, action.Tempo);
        GV("ForceOuvre" + iAct, action.ForceOuvre);
        
        // PID (avec mise à jour des valeurs affichées)
        GV("sliderKp" + iAct, action.Kp);
        GH("sensiKp" + iAct, action.Kp);
        GV("sliderKi" + iAct, action.Ki);
        GH("sensiKi" + iAct, action.Ki);
        GV("sliderKd" + iAct, action.Kd);
        GH("sensiKd" + iAct, action.Kd);
        
        GID("PID" + iAct).checked = action.PID == 1;

        // Champs séquenceur de relais
        if (action.IdxSequenceur !== undefined) {
            const selectSequenceur = GID("selectSequenceur" + iAct);
            if (selectSequenceur) selectSequenceur.value = action.IdxSequenceur;
        }
        if (action.PuissanceCharge !== undefined && action.PuissanceCharge > 0) {
            const champPuissance = GID("puissanceCharge" + iAct);
            if (champPuissance) champPuissance.value = action.PuissanceCharge;
        }

        // Configuration Pin/Sortie (OrdreOn)
        const selectPin = GID("selectPin" + iAct);
        const selectOut = GID("selectOut" + iAct);

        if (action.OrdreOn.indexOf(IS) > 0) {
            const vals = action.OrdreOn.split(IS);
            if (selectPin) selectPin.value = vals[0];
            if (selectOut) selectOut.value = vals[1];
        } else {
            // Configuration externe ou par défaut
            if (selectPin) selectPin.value = (action.OrdreOn === "") ? 0 : -1;
            if (selectOut) selectOut.value = 1;
        }

        TracePeriodes(iAct);
    }
}

/**
 * Trace les périodes horaires pour une action donnée.
 * @param {number} iAct - Index de l'action.
 */
function TracePeriodes(iAct) {
    let S = "";
    let Sinfo = "";
    let SinfoClick = "";
    let H0 = 0;
    const colors = ["#666", "#66f", "#f66", "#6f6", "#cc4"]; // NO, OFF, ON, PW, Triac
    blockEvent = false;
    
    const action = F.Actions[iAct];
    if (!action || !action.Periodes) return;

    for (let i = 0; i < action.Periodes.length; i++) {
        const periode = action.Periodes[i];
        
        // Calcul des dimensions de la période
        const w = (periode.Hfin - H0) / 24 ; // Largeur en %
        const left = H0 / 24 ; // Position en %
        H0 = periode.Hfin;
        
        const Type = periode.Type;
        const color = colors[Type];
        
        let temperature = "";
        
        // Conditions de température
        if (periode.CanalTemp >= 0) {
            if (V.temperature[periode.CanalTemp] > -100) { // La sonde de température fonctionne
                const Tsup = periode.Tsup;
                if (Tsup >= 0 && Tsup <= 1000) temperature += `<div>T &ge;${Tsup / 10}°</div>`;
                const Tinf = periode.Tinf;
                if (Tinf >= 0 && Tinf <= 1000) temperature += `<div>T &le;${Tinf / 10}°</div>`;
            }
        }
        
        // Conditions horaires d'ouverture (pour action 255/autre action)
        let H_Ouvert = "";
        if (periode.SelAct != 255) {
            if (periode.Hmin > 0) H_Ouvert += `<div>H<span class='fsize8'>ouverture</span> &ge;${Hdeci2Hmn(periode.Hmin)}</div>`;
            if (periode.Hmax > 0) H_Ouvert += `<div>H<span class='fsize8'>ouverture</span> &le;${Hdeci2Hmn(periode.Hmax)}</div>`;
            if (periode.Ooff > 0) H_Ouvert += `<div>On à Off si &le;${periode.Ooff}%</div>`;
            if (periode.O_on > 0) H_Ouvert += `<div>Off à On si &ge;${periode.O_on}%</div>`;
        }
        
        // Conditions tarifaires (Tempo, HP/HC)
        let TxtTarif = "";
        if (V.LTARFbin > 0) {
            let Tarif_ = periode.Tarif;
            let tarifText = "Tarif : ";
            if (V.LTARFbin <= 3) {
                tarifText += (Tarif_ & 1) ? "<span style='color:red;'>H. Pleine</span>" : "";
                tarifText += (Tarif_ & 2) ? "<span style='color:green;'> H. Creuse</span>" : "";
            } else {
                tarifText += (Tarif_ & 4) ? "Tempo<span style='color:blue;'>Bleu</span>" : "";
                tarifText += (Tarif_ & 8) ? "<span style='color:white;'> Blanc</span>" : "";
                tarifText += (Tarif_ & 16) ? "<span style='color:red;'> Rouge</span>" : "";
            }
            TxtTarif = `<div>${tarifText}</div>`;
        }
        
        const condition = (temperature !== "" || H_Ouvert !== "" || TxtTarif !== "") ? "<div>Condition(s) :</div>" + temperature + H_Ouvert + TxtTarif : "";
        let TexteMinMax = "";

        // Détermination du contenu de la zone d'info en fonction du type d'action/période
        if (action.Actif <= 1 && iAct > 0) {
            // Action On/Off (index > 0)
            periode.Vmax = Math.max(periode.Vmin, periode.Vmax);
            TexteMinMax = `<div>Off si Pw&gt;${periode.Vmax}W</div><div>On si Pw&lt;${periode.Vmin}W</div>${condition}`;
        } else {
            // Action de découpe sinus (Triac/PWM/Actif=0)
            periode.Vmax = Math.max(0, periode.Vmax);
            periode.Vmax = Math.min(100, periode.Vmax);
            TexteMinMax = `<div>Seuil Pw : ${periode.Vmin}W</div><div>Ouvre Max : ${periode.Vmax}%</div>${condition}`;
        }
        
        // Texte spécifique pour la découpe Triac 
        const TexteTriac = `<div>Seuil Pw : ${periode.Vmin}W</div><div>Ouvre Max : ${periode.Vmax}%</div>${condition}`;
        
        const paras = [`Pas de contr&ocirc;le`, `OFF`, `<div>ON</div><div>Ouverture : ${periode.ONouvre}%</div>` + condition, TexteMinMax, TexteTriac];
        const para = paras[Type];
        
        // Remplissage du div de la période (curseur visuel)
          // Créer la période
        S += "<div id='zone" + iAct + "_" + i + "' class='periode' data-idx='" + i + "' style='width:" + w + "%;left:" + left + "%;background-color:" + color + ";'>";
       
        // Ajouter une poignée de déplacement uniquement si ce n'est pas la dernière période
        if (i < action.Periodes.length - 1) {
            
           
            S += "<div class='handleStyle' ' data-action='" + iAct + "' data-periode='" + i + "' ";
            S += "onmousedown='startDrag(this,event," + iAct + "," + i + ");' ";
            S += "ontouchstart='startDrag(this,event," + iAct + "," + i + ");'>|||</div>";
        }
       
        S += "</div>";
        
        // Remplissage du div d'information
        const Hmn = Hdeci2Hmn(H0);
        const fs = Math.max(8, Math.min(16, w / 2)) + "px";
        Sinfo += `<div class='infoZone' style='width:${w}%;border-color:${color};font-size:${fs}' onclick='infoZclicK(${i},${iAct})'>`;
        Sinfo += `<div class='Hfin'>${Hmn}</div>${para}</div>`;
        
        SinfoClick += `<div id='info${iAct}Z${i}' class='infoZ'></div>`;
    }
    
    GH("curseurs" + iAct, S);
    // Combine les zones d'info cliquables (SinfoClick) et le contenu (Sinfo)
    GH("infoAction" + iAct, SinfoClick + Sinfo);
}

)====";

const char * ActionsJS2 = R"====(
    function startDrag(handleElement, ev, iAct, iPeriode) {
   if (ev.preventDefault) ev.preventDefault();
    if (ev.stopPropagation) ev.stopPropagation();
     draggedHandle = {
        action: iAct,
        periode: iPeriode,
        element: handleElement,
        originalStyle: handleElement.style.cssText
    };
    var dragStyle = "position:absolute;right:-15px;top:50%;transform:translateY(-50%) scale(1.2);width:30px;height:40px;";
    dragStyle += "background-color:#ffeb3b;border:2px solid #f57c00;border-radius:8px;";
    dragStyle += "cursor:ew-resize;display:flex;align-items:center;justify-content:center;";
    dragStyle += "font-size:14px;font-weight:bold;color:#333;z-index:10;";
    dragStyle += "box-shadow:0 4px 8px rgba(0,0,0,0.5);touch-action:none;user-select:none;";
    handleElement.style.cssText = dragStyle;
}

function dragHandle(ev, iAct) {
    if (!draggedHandle || draggedHandle.action !== iAct) return;
   
    if (ev.preventDefault) ev.preventDefault();
   
    var curseur = GID('curseurs' + iAct);
    var leftPos;
   
      if (ev.touches && ev.touches.length > 0) {
        leftPos = ev.touches[0].clientX - curseur.getBoundingClientRect().left;
    } else {
        leftPos = ev.clientX - curseur.getBoundingClientRect().left;
    }
   var width = curseur.getBoundingClientRect().width;
    var HeureMouse = leftPos * 2420 / width;
    var iPeriode = draggedHandle.periode;
   
    var NewHfin = Math.max(0, Math.min(HeureMouse, 2400));
   const action = F.Actions[iAct];
  if (iPeriode < action.Periodes.length - 1) {
        NewHfin = Math.min(NewHfin, action.Periodes[iPeriode + 1].Hfin);
    }
    if (iPeriode > 0) {
        NewHfin = Math.max(NewHfin, action.Periodes[iPeriode - 1].Hfin);
    }
   
   action.Periodes[iPeriode].Hfin = Math.floor(NewHfin);
   draggedHandle.element.textContent = Hdeci2Hmn(Math.floor(NewHfin));
   var H0 = (iPeriode > 0) ? action.Periodes[iPeriode - 1].Hfin : 0;
    var left = H0 / 24;
    var w = (NewHfin - H0) / 24;
   
   var zone = GID('zone' + iAct + '_' + iPeriode);
    if (zone) {
        zone.style.width = w + '%';
    }
   
     if (iPeriode < action.Periodes.length - 1) {
        var nextZone = GID('zone' + iAct + '_' + (iPeriode + 1));
        var nextLeft = NewHfin / 24;
        var nextW = (action.Periodes[iPeriode + 1].Hfin - NewHfin) / 24;
        if (nextZone) {
            nextZone.style.left = nextLeft + '%';
            nextZone.style.width = nextW + '%';
        }
    }
}

function stopDrag() {
    if (draggedHandle && draggedHandle.element) {
      draggedHandle.element.style.cssText = draggedHandle.originalStyle;
       
      var iAct = draggedHandle.action;
        TracePeriodes(iAct);
    }
    draggedHandle = null;
}

/**
 * Gère le mouvement du doigt (touch) pour ajuster les périodes.
 * @param {HTMLElement} t - L'élément curseur cliqué.
 * @param {Event} ev - L'objet événement touch.
 * @param {number} iAct - Index de l'action.
 */
function touchMove(t, ev, iAct) {
    // Calcul de la position X relative à l'élément (t)
    const leftPos = ev.touches[0].clientX - GID(t.id).getBoundingClientRect().left;
    NewPosition(t, leftPos, iAct);
}

/**
 * Gère le mouvement de la souris pour ajuster les périodes.
 * @param {HTMLElement} t - L'élément curseur cliqué.
 * @param {Event} ev - L'objet événement souris.
 * @param {number} iAct - Index de l'action.
 */
function mouseMove(t, ev, iAct) {
    if (mouseClick) {
        // Calcul de la position X relative à l'élément (t)
        const leftPos = ev.clientX - GID(t.id).getBoundingClientRect().left;
        NewPosition(t, leftPos, iAct);
    }
}


/**
 * Calcule et applique la nouvelle position d'un curseur horaire.
 * @param {HTMLElement} t - L'élément curseur.
 * @param {number} leftPos - La position X du pointeur/doigt relative à l'élément.
 * @param {number} iAct - Index de l'action.
 */
function NewPosition(t, leftPos, iAct) {
    
    // Récupère la largeur totale de la zone de glissement
    const width = GID(t.id).getBoundingClientRect().width;
    
    // Convertit la position en centièmes d'heure (0 à 2400)
    // Utilisation de 2400 comme référence pour 24 heures (le 2420 est ajusté au standard 2400)
    const HeureMouse = leftPos * 2400 / width;
    
    let idxClick = 0;
    let deltaX = 999999;
    
    // Cherche le curseur le plus proche (sauf le dernier qui doit rester à 2400)
    for (let i = 0; i < F.Actions[iAct].Periodes.length - 1; i++) {
        const dist = Math.abs(HeureMouse - F.Actions[iAct].Periodes[i].Hfin);
        if (dist < deltaX) {
            idxClick = i;
            deltaX = dist;
        }
    }
    
    // Borne la nouvelle position entre 0 et 2400
    let NewHfin = Math.max(0, Math.min(HeureMouse, 2400));
    
    // La dernière période (qui représente 24h) ne doit pas être déplacée
    if (idxClick === F.Actions[iAct].Periodes.length - 1) {
        NewHfin = 2400;
    }
    
    // S'assure que le curseur ne dépasse pas le curseur suivant (à droite)
    if (idxClick < F.Actions[iAct].Periodes.length - 1) {
        NewHfin = Math.min(NewHfin, F.Actions[iAct].Periodes[idxClick + 1].Hfin);
    }
    
    // S'assure que le curseur ne recule pas au-delà du curseur précédent (à gauche)
    // Note: Pour le premier curseur (idxClick = 0), cette condition est ignorée.
    if (idxClick > 0) {
        // Le curseur précédent est celui qui se termine là où celui-ci commence.
        // On vérifie la fin du curseur précédent (idxClick - 1)
        NewHfin = Math.max(NewHfin, F.Actions[iAct].Periodes[idxClick - 1].Hfin);
    }
    
    // Affecte la nouvelle fin de période (arrondie à l'entier pour les centièmes d'heure)
    F.Actions[iAct].Periodes[idxClick].Hfin = Math.floor(NewHfin);
    
    // Redessine l'affichage des périodes
    TracePeriodes(iAct);
}

/**
 * Ajoute ou supprime une période horaire pour une action.
 * @param {number} v - 1 pour ajouter, -1 pour supprimer.
 * @param {number} iAct - Index de l'action.
 */
function AddSub(v, iAct) {
    if (v === 1) {
        // Ajout
        let type=3; //Pw pour les relais
        if (iAct==0) type=4; //Pw pour le Triac
        if (F.Actions[iAct].Periodes.length < 8) {
            // Crée la nouvelle période avec des valeurs par défaut
            const newPeriod = {
                Hfin: 2400,
                Type: type,
                Vmin: 0,
                Vmax: 100,
                ONouvre : 100,
                Tinf: 1600,
                Tsup: 1600,
                Hmin: 0,
                Hmax: 0,
                CanalTemp: -1,
                SelAct: 255,
                Ooff: 0,
                O_on: 0,
                Tarif: 31 // Tarif codé en bits (toutes les options actives par défaut)
            };
            F.Actions[iAct].Periodes.push(newPeriod);
            
            // Recalcule et ajuste Hfin de l'avant-dernière période pour centrer la nouvelle
            let Hbas = 0;
            const len = F.Actions[iAct].Periodes.length;
            if (len > 2) {
                // Hbas est l'Hfin de la période précédant l'avant-dernière
                Hbas = parseInt(F.Actions[iAct].Periodes[len - 3].Hfin, 10);
            }
            if (len > 1) {
                // Ajuste Hfin de l'avant-dernière période (celle qui vient d'être créée devient la dernière)
                F.Actions[iAct].Periodes[len - 2].Hfin = Math.floor((Hbas + 2400) / 2);
            }
        }
    } else {
        // Suppression
        if (F.Actions[iAct].Periodes.length > 1) {
            F.Actions[iAct].Periodes.pop();
            // S'assure que la nouvelle dernière période se termine à 2400
            if (F.Actions[iAct].Periodes.length > 0) {
                F.Actions[iAct].Periodes[F.Actions[iAct].Periodes.length - 1].Hfin = 2400;
            }
        }
    }
    F.Actions[iAct].NbPeriode= F.Actions[iAct].Periodes.length;
    TracePeriodes(iAct);
}

/**
 * Ouvre le panneau de configuration pour une période donnée.
 * @param {number} i - Index de la période.
 * @param {number} iAct - Index de l'action.
 */
function infoZclicK(i, iAct) {
    let capteurT = false;
    
    // Empêche l'ouverture multiple du panneau
    if (!blockEvent) {
        blockEvent = true;
        
        const periode = F.Actions[iAct].Periodes[i];
        const Type = periode.Type;
        const idZ = "info" + iAct + "Z" + i;
        const ONouvre = periode.ONouvre;
        
        let S = "<div class='selectZ'> S&eacute;lection Action<div class='closeZ' onclick='infoZclose(\"" + idZ + "\")'>X</div></div>";
        
        // --- Choix du Mode ---
        
        // OFF
        let check = (Type === 1) ? "checked" : "";
        S += "<div class='zOff'><div class='radioC'><input type='radio' name='R" + idZ + "' onclick='selectZ(1," + i + "," + iAct + ");' " + check + " title='Off forcé'>OFF</div></div>";
        
        S += "<div class='fcontainer'><div class='fcontleft'>";
        
        // ON (100%)
        check = (Type === 2) ? "checked" : "";
        S += "<div class='zOn'><div class='radioC'><input type='radio' name='R" + idZ + "' onclick='selectZ(2," + i + "," + iAct + ");' " + check + " title='On forcé (si conditions optionnelles valides)'>ON </div>";
        S += "<div><small>Ouverture: &nbsp;</small><input id='ONouvre_" + idZ + "' type='number' value='" + ONouvre + "' onchange='NewVal(this)' title='Ouverture en position ON.' >%</div></div>";
        
        // Routage/PWM/Triac
        check = (Type > 2) ? "checked" : "";
        
        const Vmin = periode.Vmin;
        const Vmax = periode.Vmax;
        const Tinf = periode.Tinf;
        const Tsup = periode.Tsup;
        
        let TinfC = Tinf / 10;
        let TsupC = Tsup / 10;
        
        const Hmin = (periode.Hmin > 0) ? Hdeci2Hmn(periode.Hmin) : "";
        const Hmax = (periode.Hmax > 0) ? Hdeci2Hmn(periode.Hmax) : "";
        const Ooff = (periode.Ooff > 0) ? periode.Ooff : "";
        const O_on = (periode.O_on > 0) ? periode.O_on : "";
        
        // Vider si hors de la plage -50.0 à 150.0
        if (Tinf > 1500 || Tinf < -500) TinfC = ""; 
        if (Tsup > 1500 || Tsup < -500) TsupC = ""; 

        if (iAct > 0) {
            // Mode Routage ON/OFF ou Multi/Train de sinus
            const Routage = ["", "Routage ON/Off", "Routage Multi-sinus", "Routage Train de Sinus", "PWM", "Routage Demi-Sinus", "Routage Séquenceur"];
            S += "<div class='zPw'><div class='radioC'><input type='radio' name='R" + idZ + "' onclick='selectZ(3," + i + "," + iAct + ");' " + check + ">" + Routage[F.Actions[iAct].Actif] + "</div>";
            
            if (F.Actions[iAct].Actif <= 1) {
                // Routage ON/OFF
                S += "<div><small>On : &nbsp;</small>Pw &lt;<input id='Pw_min_" + idZ + "' type='number' value='" + Vmin + "' onchange='NewVal(this)' title='Seuil de puissance pour activer ou désactiver le routage. Attention, en cas de mode On/Off la diff&eacute;rence, seuil sup&eacute;rieur moins seuil inf&eacute;rieur doit &ecirc;tre sup&eacute;rieure &agrave; la consommation du dipositif pour &eacute;viter l&apos;oscillation du relais de commande.'>W</div>";
                S += "<div><small>Off : </small>Pw &gt;<input id='Pw_max_" + idZ + "' type='number' value='" + Vmax + "' onchange='NewVal(this)'>W</div>";
                S += "<div><small>Puissance active en entrée de maison</small></div></div>";
            } else {
                // Routage Multi/Train de sinus/PWM
                S += "<div><small>Seuil Pw : &nbsp;</small><input id='Pw_min_" + idZ + "' type='number' value='" + Vmin + "' onchange='NewVal(this)' title='Seuil de puissance pour activer ou désactiver le routage.' >W</div>";
                S += "<div><small>Puissance active en entrée de maison</small></div>";
                S += "<div><small>Ouvre Max : </small><input id='Pw_max_" + idZ + "' type='number' value='" + Vmax + "' onchange='NewVal(this)' title='Ouverture maximum du SSR. Valeur typique : 100%'>%</div></div>";
            }
        } else {
            // Mode Triac/Découpe Sinus (iAct === 0)
            const Routage = ["", "Routage Découpe Sinus", "Routage Multi-sinus", "Routage Train de Sinus", "", "Routage Demi-Sinus", ""];
            S += "<div class='zTriac'><div class='radioC'><input type='radio' name='R" + idZ + "' onclick='selectZ(4," + i + "," + iAct + ");' " + check + ">" + Routage[F.Actions[iAct].Actif] + "</div>";
            S += "<div>Seuil Pw &nbsp;<input id='Pw_min_" + idZ + "' type='number' value='" + Vmin + "' onchange='NewVal(this)' title='Seuil en W de r&eacute;gulation par le Triac de la puissance mesur&eacute;e Pw en entrée de la maison. Valeur typique : 0.'>W</div>";
            S += "<div><small>Puissance active en entrée de maison</small></div>";
            S += "<div>Ouvre Max <input id='Pw_max_" + idZ + "' type='number' value='" + Vmax + "' onchange='NewVal(this)' title='Ouverture maximum du triac. Valeur typique : 100%'>%</div></div>";
        }
        
        S += "</div>"; // Ferme fcontleft
        
        // --- Conditions optionnelles (Température, Action, Tarif) ---
        
        // Sélecteur de canal de température
        let SelectT = "<div>Canal de Température <select id='CanalTemp" + idZ + "' onchange='NewVal(this)'><option value=-1 selected>Non exploité</option>";
        for (let c = 0; c < 4; c++) {
            if (V.temperature[c] > -100) {
                const Temper = parseFloat(V.temperature[c]).toFixed(1);
                SelectT += "<option value=" + c + " >" + F["nomTemperature" +c] + " (" + Temper + "°)" + "</option>";
                capteurT = true;
            }
        }
        SelectT += "</select></div>";
        
        // Affichage des conditions optionnelles
        const styleDisplay = (F.ModePara === 0) ? "none" : "block";
        const style = `style='display:${styleDisplay}';`;
        S += "<div>";
        S += "<div class='TitZone' " + style + ">&nbsp;&nbsp;&nbsp;Conditions optionnelles pour activer</div>";
        
        // Conditions de Température
        if (capteurT) {
            S += "<div class='bord1px' " + style + ">";
            S += SelectT;
            S += "<div class='minmax'><div>T &ge;<input id='T_sup_" + idZ + "' type='number' value='" + TsupC + "' onchange='NewVal(this)' title='Définir la ou les températures qui permettent l&apos;activation de la fonction On ou Routage.'>°</div>";
            S += "<div>T &le;<input id='T_inf_" + idZ + "' type='number' value='" + TinfC + "' onchange='NewVal(this)'>°</div></div>";
            S += "<div><small>T en degré (-50.0 à 150.0) ou laisser vide</small></div>";
            S += "</div>";
        }
        
        // Conditions d'Action
        S += "<div class='bord1px' " + style + " >";
        S += "<div>Etat d'une Action <select id='SelAct" + idZ + "' onchange='NewVal(this)'>" + SelectActions + "</select></div>";
        S += "<div class='minmax'><div>Durée : </div><div>H &ge;<input id='H_min_" + idZ + "' type='text' value='" + Hmin + "' onchange='NewVal(this)'>h:mn</div>";
        S += "<div>H &le;<input id='H_max_" + idZ + "' type='text' value='" + Hmax + "' onchange='NewVal(this)'>h:mn</div></div>";
        S += "<div class='minmax'><div>Seuil : </div><div>On à Off si &le;<input id='O_min_" + idZ + "' type='number' value='" + Ooff + "' onchange='NewVal(this)'>%</div>";
        S += "<div>Off à On si &ge;<input id='O_max_" + idZ + "' type='number' value='" + O_on + "' onchange='NewVal(this)'>%</div></div>";
        S += "<div><small>h:mn ou % ou laisser vide</small></div>";
        S += "</div>";


        // Conditions Tarifaires
        if (V.LTARFbin > 0) {
            // Récupère l'état actuel du tarif
            const Tarif_ = F.Actions[iAct].Periodes[i].Tarif;
            const isHPle = (Tarif_ & 1);
            const isHCre = (Tarif_ & 2);
            const isBe = (Tarif_ & 4);
            const isBa = (Tarif_ & 8);
            const isRo = (Tarif_ & 16);

            S += "<div class='bord1px'>";
            S += "<div title='Condition d&apos;activation suivant la tarification. Sinon ordre Off envoyé ou Triac/SSR se ferme.'>Actif si tarif :</div>";
            if (V.LTARFbin <= 3) {
                // HP/HC
                S += "<div id='PleineCreuse'><span style='color:red;'>Heure Pleine</span><input type='checkbox' " + (isHPle ? "checked" : "") + " id='TarifPl_" + idZ + "' onchange='NewVal(this)'> <span style='color:green;'>Heure Creuse</span><input type='checkbox' " + (isHCre ? "checked" : "") + " id='TarifCr_" + idZ + "' onchange='NewVal(this)'></div>";
            } else {
                // Tempo
                S += "<div id='Tempo'>Tempo <span style='color:blue;'>Bleu</span><input type='checkbox' " + (isBe ? "checked" : "") + " id='TarifBe_" + idZ + "' onchange='NewVal(this)'><span style='color:white;'> Blanc</span><input type='checkbox' " + (isBa ? "checked" : "") + " id='TarifBa_" + idZ + "' onchange='NewVal(this)'><span style='color:red;'> Rouge</span><input type='checkbox' " + (isRo ? "checked" : "") + " id='TarifRo_" + idZ + "' onchange='NewVal(this)'></div>";
            }
            S += "</div>";
        }
        S += "</div>"; // Ferme le bloc conditions
        
        S += "</div>"; // Ferme la div container

        GH(idZ, S);
        
        // --- Affectation des valeurs aux sélecteurs (après construction du HTML) ---
        if (capteurT) GID("CanalTemp" + idZ).value = periode.CanalTemp;
        GID("SelAct" + idZ).value = periode.SelAct;
        GID(idZ).style.display = "block";
    }
}

)====";

const char * ActionsJS3 = R"====(
/**
 * Ferme le panneau d'information de la zone cliquée.
 * @param {string} idx - L'ID de l'élément à fermer (e.g., "info0Z0").
 */
function infoZclose(idx) {
    const champs = idx.split("info");
    const idx_parts = champs[1].split("Z");
    const iAct = parseInt(idx_parts[0], 10);
    
    const element = GID(idx);
    if (element) {
        element.style.display = "none";
    }

    // Utilisation d'une fonction anonyme pour un appel sécurisé et déverrouillage
    setTimeout(() => {
        blockEvent = false; // Réinitialise le verrou
        TracePeriodes(iAct);
    }, 100);
}

/**
 * Gère la sélection du type de fonctionnement (OFF, ON, Routage...) pour une période.
 * @param {number} T - Nouveau type de période (1=OFF, 2=ON, 3+=Routage/PID...).
 * @param {number} i - Index de la période.
 * @param {number} iAct - Index de l'action.
 */
function selectZ(T, i, iAct) {
    if (F.Actions[iAct] && F.Actions[iAct].Periodes[i]) {
        if (F.Actions[iAct].Periodes[i].Type != T) {
            F.Actions[iAct].Periodes[i].Type = T;
            const idZ = "info" + iAct + "Z" + i;
            
            // Si le mode est OFF (1), on ferme et on rafraîchit.
            if (T === 1) { 
                infoZclose(idZ);
            } else {
                // Pour les modes actifs (ON, Routage), on rafraîchit l'affichage
                // car le contenu de la zone peut changer (changement de couleur/texte)
                TracePeriodes(iAct);
            }
        }
    }
}

/**
 * Met à jour la valeur d'un paramètre spécifique d'une période après modification dans un champ input.
 * @param {HTMLElement} t - L'élément input/select qui a déclenché l'événement (this).
 */
function NewVal(t) {
    const champs = t.id.split("info");
    const idx = champs[1].split("Z"); // Num Action, Num période
    const iAct = parseInt(idx[0], 10);
    const i = parseInt(idx[1], 10);
    
    if (!F.Actions[iAct] || !F.Actions[iAct].Periodes[i]) return;

    const periode = F.Actions[iAct].Periodes[i];
    const value = GID(t.id).value;

    if (champs[0].indexOf("Pw_min") >= 0) {
        periode.Vmin = Math.floor(value);
    } else if (champs[0].indexOf("Pw_max") >= 0) {
        periode.Vmax = Math.floor(value);
        if (iAct === 0) { // Logique spécifique pour l'Action Triac (index 0)
            periode.Vmax = Math.max(periode.Vmax, 5);
            periode.Vmax = Math.min(periode.Vmax, 100);
        }
    } else if (champs[0].indexOf("ouvre") > 0) { // Ouverture ON
        periode.ONouvre = Math.floor(value);
    } else if (champs[0].indexOf("inf") > 0) { // Température inférieure (Tinf)
        let V = value;
        if (V === "") V = 158; 
        periode.Tinf = Math.floor(parseFloat(V) * 10);
    } else if (champs[0].indexOf("sup") > 0) { // Température supérieure (Tsup)
        let V = value;
        if (V === "") V = 158; 
        periode.Tsup = Math.floor(parseFloat(V) * 10);
    } else if (champs[0].indexOf("H_min") >= 0) {
        periode.Hmin = Hmn2Hdeci(value);
    } else if (champs[0].indexOf("H_max") >= 0) {
        periode.Hmax = Hmn2Hdeci(value);
    } else if (champs[0].indexOf("O_min") >= 0) {
        periode.Ooff = Math.max(0, Math.min(100, Math.floor(value)));
    } else if (champs[0].indexOf("O_max") >= 0) {
        periode.O_on = Math.max(0, Math.min(100, Math.floor(value)));
    } else if (champs[0].indexOf("Tarif") >= 0) {
        const idZ = "info" + champs[1];
        let Tarif_ = 0;
        
        // Récupère l'état des checkboxes et construit la valeur binaire du tarif
        if (V.LTARFbin <= 3) { // HP/HC
            Tarif_ += GID("TarifPl_" + idZ).checked ? 1 : 0; // H pleine (bit 0)
            Tarif_ += GID("TarifCr_" + idZ).checked ? 2 : 0; // H creuse (bit 1)
        } else { // Tempo
            Tarif_ += GID("TarifBe_" + idZ).checked ? 4 : 0;  // Bleu (bit 2)
            Tarif_ += GID("TarifBa_" + idZ).checked ? 8 : 0;  // Blanc (bit 3)
            Tarif_ += GID("TarifRo_" + idZ).checked ? 16 : 0; // Rouge (bit 4)
        }
        periode.Tarif = Tarif_;
    } else if (champs[0].indexOf("CanalTemp") >= 0) {
        periode.CanalTemp = parseInt(value, 10);
    } else if (champs[0].indexOf("SelAct") >= 0) {
        periode.SelAct = parseInt(value, 10);
    }
}

/**
 * Active le mode édition pour le titre d'une action.
 * @param {number} iAct - Index de l'action.
 */
function editTitre(iAct) {
    const titreElement = GID("titre" + iAct);
    if (titreElement && titreElement.innerHTML.indexOf("<input") === -1) {
        GH("titre" + iAct, "<input type='text' value='" + titreElement.innerHTML + "' id='Etitre" + iAct + "' onblur='TitreValid(" + iAct + ")'>");
        const input = GID("Etitre" + iAct);
        if (input) input.focus();
    }
}

/**
 * Valide le nouveau titre et réaffiche le label.
 * @param {number} iAct - Index de l'action.
 */
function TitreValid(iAct) {
    const input = GID("Etitre" + iAct);
    if (input) {
        F.Actions[iAct].Titre = input.value.trim();
        GH("titre" + iAct, F.Actions[iAct].Titre);
    }
}

/**
 * Vérifie et applique l'état "disabled" ou "visible" aux éléments
 * en fonction du mode d'action (`Actif`) et du choix de sortie (GPIO ou Externe).
 */
function checkDisabled() {

    const show = (id, disp) => GID(id).style.display = disp;
    const val  = id => GID(id).value;
    const chk  = id => GID(id).checked;

    show("SelectOut0", "none");
    show("SelectPin0", "none");
    show("Freq_PWM", "none");
    show("commun", (F.ModePara > 0 && F.ReacCACSI < 100) ? "block" : "none");

    for (let iAct = 0; iAct < F.Actions.length; iAct++) {

        // --- Détermination du mode actif ---
        for (let m = 0; m <= 6; m++) {
            if (chk(`radio${iAct}-${m}`)) {
                F.Actions[iAct].Actif = m;
            }
        }

        const pinVal = parseInt(val(`selectPin${iAct}`), 10);

        // Forçage en mode OnOff si pas de pin, sauf si mode Séquenceur (actif=6) ou mode Inactif (actif=0) logique Triac (iAct=0)
        if (pinVal === -1 && F.Actions[iAct].Actif > 1 && F.Actions[iAct].Actif !== 6 && iAct > 0) {
            F.Actions[iAct].Actif = 1;
            GID(`radio${iAct}-1`).checked = true;
        }

        // Mise à jour planning
        TracePeriodes(iAct);  // Attention: TracePeriodes utilise F.Actions[iAct].Actif pour adapter l'affichage, il faut donc mettre à jour Actif avant d'appeler TracePeriodes

        const triac = (F.pTriac > 0) ? "block" : "none";
        show("planning0", triac);
        show("TitrTriac", triac);

        const actif = F.Actions[iAct].Actif;
        const estActif = (actif > 0);
        const estOnOff = (actif === 1 && iAct > 0);      // Mode On/Off simple -> pas de visu ni de graph, temporisation au lieu d'ouverture forcée, pas de relais séquenceur possible
        const estOnOffExterne = estOnOff && pinVal < 0;  // pin<0 = mode Externe choisi -> désactive le reglage du niveau de sortie et affiche les options de host/port/ordreOnOff
        const estSequenceur = (actif === 6 && iAct > 0); // Mode Séquenceur -> désactive les options de pin et de niveau de sortie, affiche le panneau de choix du séquenceur parent


        show(`blocPlanning${iAct}`, estActif ? "block" : "none"); // Masque le bloc de planning complet si mode Inactif selectionné
        show(`groupeSequenceur${iAct}`, estSequenceur ? "flex" : "none"); // Affiche le bloc informatif Séquenceur de relais uniquement si mode Séquenceur sélectionné

        const sequenceursExistants = [];  // Liste des index des actions configurées en mode Séquenceur (pour alimenter le select de choix du relais séquenceur)
        for (let m = 0; m < F.Actions.length; m++) {
            const r6m = GID(`radio${m}-6`);
            if (r6m && r6m.checked) sequenceursExistants.push(m);
        }
        // Affiche le choix de relais séquenceur uniquement si au moins un séquenceur existe et que le mode Séquenceur n'est pas sélectionné (un séquenceur ne peut pas être relais d'un autre séquenceur) et que le mode On/Off simple n'est pas sélectionné (pas de relais séquenceur possible en mode On/Off simple)
        const peutEtreRelaisSequence = (sequenceursExistants.length > 0 && !estSequenceur && !estOnOff && iAct > 0);
        show(`relaisSequence${iAct}`, peutEtreRelaisSequence ? "flex" : "none");

        const selectSequenceur = GID(`selectSequenceur${iAct}`);
        if (selectSequenceur) {
            // Reconstruction des options du select de relais séquenceur à chaque changement de mode pour refléter les séquenceurs disponibles
            // à partir du DOM (et pas à partir de F.Actions qui n'a pas encore été synchronisé pour tous les indexes)
            let opts = "<option value='-1'>\u2014 aucun \u2014</option>";
            for (let s = 0; s < sequenceursExistants.length; s++) {
                const idxSeq = sequenceursExistants[s];
                const titre = (F.Actions[idxSeq] && F.Actions[idxSeq].Titre) ? F.Actions[idxSeq].Titre : ("Action " + idxSeq);
                opts += `<option value='${idxSeq}'>${titre}</option>`;
            }

            // Check index de séquenceur valide, et met à jour F.Actions[iAct].IdxSequenceur en conséquence
            if (!peutEtreRelaisSequence) {
                // Si le relais ne peut plus etre séquencé, on force la valeur à -1 (aucun) et on met à jour F.Actions pour éviter d'avoir un index de séquenceur non valide
                selectSequenceur.value = "-1";
            } else {
                // Sinon, on restaure la sélection précédente si elle est toujours valide (sinon le navigateur mettra automatiquement la valeur à -1 qui correspond à "aucun")
                const savedVal = selectSequenceur.value;
                selectSequenceur.innerHTML = opts;
                selectSequenceur.value = savedVal;  // restaure la sélection (-> -1 si le séquenceur a disparu)
                selectSequenceur.value = selectSequenceur.value; // forcer le bon display affichée en cas de restauration à -1
            }
        }
        
        const estRelaisSequence = (selectSequenceur && selectSequenceur.value != "-1"); // Relais séquencée (=séquenceur parent sélectionné) -> désactive les options de periodes/regulation/visu

        show(`planningPeriodes${iAct}`, (estActif && !estRelaisSequence) ? "block" : "none");        // Pas de periodes de planning en mode séquencé (le séquenceur parent gère lui même son planning)
        show(`planningControles${iAct}`, (estActif && !estRelaisSequence) ? "flex" : "none");        // Pas de periodes de planning en mode séquencé (le séquenceur parent gère lui même son planning)
        show(`visu${iAct}`, (estActif && !estOnOff && !estRelaisSequence) ? "block" : "none");       // Pas de visu ni de graph en mode On/Off simple ou en mode séquencé
        show(`forceOuvre${iAct}`, (estActif && !estOnOff) ? "block" : "none");  // Pas de réglage d'ouverture forcée en mode On/Off
        show(`Tempo${iAct}`, estOnOff ? "block" : "none");                      // Affiche reglage Temporisation uniquement en mode On/Off

        if (estOnOff || estRelaisSequence) { // Cache le graph dès qu'on passe en mode On/Off ou relais séquencé
            show(`graphAction${iAct}`, "none");
        }

        show(`SelectPin${iAct}`, estSequenceur ? "none" : "inline-block");  // Masque le choix de pin si mode séquenceur sélectionné (un séquenceur n'agit pas directement sur une pin)
        show(`SelectOut${iAct}`, (estSequenceur || estOnOffExterne) ? "none" : "inline-block"); // Masque le choix du niveau de sortie On si mode séquenceur sélectionné (un séquenceur n'agit pas directement sur une pin) ou si mode On/Off Externe sélectionné (pas de pin physique en mode On/Off Externe)

        // Affiche les options de host et d'order On/Off uniquement en mode OnOff Externe
        let dispOnOffExterne = estOnOffExterne ? "block" : "none";
        show(`Host${iAct}`, dispOnOffExterne);
        show(`Port${iAct}`, dispOnOffExterne);
        show(`Repet${iAct}`, dispOnOffExterne);
        show(`ordreoff${iAct}`, dispOnOffExterne);
        show(`ordreon${iAct}`, dispOnOffExterne);

        // Désactivation des modes 2..6 si pas de pin, ie si mode externe choisi
        for (const mode of [2, 3, 4, 5, 6]) {
            const r = GID(`radio${iAct}-${mode}`);
            if (r) r.disabled = estOnOffExterne;
        }

        // Correction d'ordreOn si IS présent alors qu'aucune pin
        const ordreOn = GID(`ordreOn${iAct}`);
        if (ordreOn && pinVal === -1 && ordreOn.value.indexOf(IS) > 0) {
            ordreOn.value = "";
        }

        // Ligne basse
        show(
            `ligne_bas${iAct}`,
            (actif === 1 && pinVal <= 0 && iAct > 0) ? "flex" : "none"
        );

        // Slider Triac
        show(
            `fen_slide${iAct}`,
            (actif === 1 && iAct > 0) ? "none" : "table"
        );

        // PWM → affichage Freq et Commun
        if (chk(`radio${iAct}-4`)) {
            show("Freq_PWM", "block");
            show("commun", "block");
        }

        // PID
        const pid = GID(`PID${iAct}`);
        if (F.ModePara === 0 && pid) pid.checked = false;

        show(`PIDbox${iAct}`, (F.ModePara === 0 || estOnOff || estRelaisSequence) ? "none" : "block");

        const pidActive = pid && pid.checked;
        show(`Propor${iAct}`, pidActive ? "table-row" : "none");
        show(`Derive${iAct}`, pidActive ? "table-row" : "none");
    }

    // PWM et Sequenceur désactivés sur triac
    show("Pwm0", "none");
    show("Sequenceur0", "none");
}


/**
 * Etabli les configurations d'actions 
 */
function TraceActions(rajout) {
        
            if (F.Actions.length == 0) {  //Action 0 Triac au mini
                F.Actions[0] = CreerAction(0, "Titre Triac");
            }
            if (F.Actions.length == 1) {  //Action 1 relais au mini au mini
                F.Actions[1] = CreerAction(1, "Titre Relais 1");
            }
            let Imax=F.Actions.length;
            if (rajout) F.Actions[Imax] = CreerAction(Imax, "Titre Relais " + Imax);

            var S = "";
            for (var i = 1; i < F.Actions.length; i++) {
                S += "<div id='planning" + i + "' class='planning' ></div>";
            }
           

            if (F.Actions.length<10) S += "<input id='butR' type='button'  class='tbut' value='+' onclick='TraceActions(true);' title='Rajouter un relais d&apos;action.'>";
            GH("plannings", S);
            for (var iAct = 0; iAct < F.Actions.length; iAct++) {
                TracePlanning(iAct);
            }
            checkDisabled();
 
}

/**
 * Envoie les coefficients PID au serveur en temps réel (uniquement si PlotIdx >= 0).
 * Ces coefficients ne sont pas enregistrés.
 * @param {number} iAct - Index de l'action.
 */
function UpdateK(iAct) {
    if (PlotIdx >= 0) {
        const Kp = Math.floor(GID("sliderKp" + iAct).value);
        const Ki = Math.floor(GID("sliderKi" + iAct).value);
        const Kd = Math.floor(GID("sliderKd" + iAct).value);

        const xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                // var retour = this.responseText; // Le retour n'est pas utilisé
            }
        };

        xhttp.open('GET', `/UpdateK?iAct=${iAct}&Kp=${Kp}&Ki=${Ki}&Kd=${Kd}`, true);
        xhttp.send();
    }
}
)====";

const char * ActionsJS4 = R"====(

function Send_Values(){
  
    GID("attente").style.visibility = "visible"; 

    // 1. Mise à jour du modèle  depuis le DOM
    for (let iAct = 0; iAct < F.Actions.length; iAct++) {
        const action = F.Actions[iAct];
        
        // S'assurer que tous les modes sont couverts (0 à 6)
        for (let i = 0; i <= 6; i++) {
            const radio = GID(`radio${iAct}-${i}`);
            if (radio && radio.checked) {
                action.Actif = i;
            }
        }
        
        // Conversion explicite en entier pour les nombres
        action.Action=iAct; //Pour info numéro Action
        action.Titre = GID("titre" + iAct).innerHTML.trim();
        action.Host = GID("host" + iAct).value.trim();
        action.Port = parseInt(GID("port" + iAct).value, 10) || 0; // Sécurité: valeur par défaut 0
        action.OrdreOn = GID("ordreOn" + iAct).value.trim();
        action.OrdreOff = GID("ordreOff" + iAct).value.trim();
        action.Repet = parseInt(GID("repet" + iAct).value, 10) || 0;
        action.Tempo = parseInt(GID("tempo" + iAct).value, 10) || 0;
        action.ForceOuvre = parseInt(GID("ForceOuvre" + iAct).value, 10) || 100;

        // Séquenceur de relais
        const selectSequenceur = GID("selectSequenceur" + iAct);
        action.IdxSequenceur = selectSequenceur ? (parseInt(selectSequenceur.value, 10) || -1) : -1;
        const champPuissance = GID("puissanceCharge" + iAct);
        action.PuissanceCharge = champPuissance ? (parseInt(champPuissance.value, 10) || 0) : 0;

        // Coefficients PID
        action.Kp = parseInt(GID("sliderKp" + iAct).value, 10) || 0;
        action.Ki = parseInt(GID("sliderKi" + iAct).value, 10) || 0;
        action.Kd = parseInt(GID("sliderKd" + iAct).value, 10) || 0;
        action.PID = GID("PID" + iAct).checked ? 1 : 0;
        
        const selectPin = GID("selectPin" + iAct);
        if (selectPin && selectPin.value >= 0 && action.Actif !== 6) {
            // Reconstruit OrdreOn pour la sortie GPIO/relais (Pin + Sortie)
            const selectOut = GID("selectOut" + iAct);
            action.OrdreOn = selectPin.value + IS + selectOut.value;
        }

         for (let i = 0; i < action.Periodes.length; i++) {
                const periode = action.Periodes[i];
                
                // Logique spécifique au mode Standard (non Routage)
                if (F.ModePara == 0) { 
                    periode.CanalTemp = -1;
                    periode.SelAct = 255;
                }
                
                
            }
        
        // Logique d'effacement de l'action (iAct > 0)
        // Un séquenceur (Actif=6) n'a pas besoin de GPIO, il ne doit pas être supprimé si selectPin=0.
        if (iAct > 0 && (action.Titre === "" || (selectPin && selectPin.value == 0 && action.Actif !== 6))) { 
            action.Actif = -1; // Marque l'action à effacer 
        }
        action.NbPeriode = action.Periodes.length;
    }
    
    // 2. Effacement des Actions inutilisées
    let j=0 ;
    for (let iAct = 0; iAct < F.Actions.length; iAct++) {
        if (F.Actions[iAct].Actif >= 0) { // N'envoie que les actions valides ou actives
            if(j!=iAct) F.Actions[j]=F.Actions[iAct];
            F.Actions[j].Action=j;
            j++;
        }
    }
    
    F.Actions=F.Actions.slice(0,j);
   
    // 3. Récupération des paramètres globaux
    F.Fpwm = document.querySelector('input[name="Fpwm"]:checked').value;
    F.ReacCACSI = document.querySelector('input[name="ReacCACSI"]:checked').value;
    F.NbActions=F.Actions.length;
    // 4. Envoi POST
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
        location.reload();
      })
      .catch(error => {
        console.error("Erreur d'envoi :", error);
      });
    
}
function Plot(iAct) {
    for (i = 0; i < F.Actions.length; i++) {
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
            
            if (F.ModePara == 1 && GID("PID" + iAct).checked ){
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
             if (F.ModePara == 1 && GID("PID" + iAct).checked){
                    S += SD  + SP;
             }
            S +=  SI  + SR + SO + SE + "</svg>";
            GID('graphSVG' + iAct).innerHTML = S;

        }
        setTimeout(() => {
            PlotR(iAct);
        }, 500);
    }
}

function ShowAction() {
    
    let Dt = 200; //ms
    if (PlotIdx >= 0) {
        const xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                const retour = this.responseText;
                // Supposons que MesuresPwAct est un tableau global
                MesuresPwAct = retour.split(RS).map(val => isNaN(parseFloat(val)) ? 0 : parseFloat(val)); // Assure la conversion numérique et la sécurité
            }
        };
        xhttp.open('GET', '/ShowAction?NumAction=' + PlotIdx, true);
        xhttp.send();
    } else {
        // Initialisation propre des tableaux si PlotIdx < 0
        MesuresPwAct = [0, 0, 0, 0];
        
        // Initialisation de toutes les valeurs du graphe à zéro
        // (En supposant que Ecart, Ouvert, Retard, Prop, Integ, Deriv sont des tableaux globaux de taille 150)
        const zeros = new Array(150).fill(0);
        Ecart = [...zeros];
        Ouvert = [...zeros];
        Retard = [...zeros];
        Prop = [...zeros];
        Integ = [...zeros];
        Deriv = [...zeros];
        
        Dt = 2000;
    }
    
    setTimeout(() => {
        ShowAction();
    }, Dt);
}

//=============Fin Initialisation  ===============
function SetParaFixe(){
    LoadParaVar();
    Set_Couleurs();
    ShowAction();

    if (F.ReacCACSI < 100) {
        GID("CACSI" + F.ReacCACSI).checked = true; //Reactivité Ki CACSI et non Estimation
        GID("CACSI").style = "display:block;";
    }
    GID("Fpwm" + F.Fpwm).checked = true;
}


function SetParaVar() {
    SelectActions = "<option value=255>Non exploité</option>";
    for ( let esp = 0; esp < nomRMS.length; esp++) { //Liste des actions par routeur
        for (var iAct = 0; iAct < nomActions[esp].length; iAct++) {
            var v = esp * 10 + parseInt(nomActions[esp][iAct][0]); //Nombre refletant la référence esp et action
            var T = (esp == 0) ? "" : nomRMS[esp] + " / ";
            SelectActions += "<option value=" + v + ">" + T + nomActions[esp][iAct][1] + "</option>";
            ListeActions[v] = T + nomActions[esp][iAct][1];
        }
    }
    TraceActions(false);
}



function AdaptationSource() {

}
)====";