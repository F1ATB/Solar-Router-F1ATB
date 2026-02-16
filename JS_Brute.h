//*********************************************
// Page données RMS Brutes  Javascript 
//*********************************************


const char *PageBruteJS1 = R"====(
      // ============================================================================
// Variables globales
// ============================================================================

let InitFait = false;
let IdxMessage = 0;
let MessageLinky = "";
const BordsInverse = [".Bbrut"];

// ============================================================================
// Tableaux des mesures UxIx2
// ============================================================================
const M = [
  ["Tension_M", "Tension efficace", "V", "V"],
  ["Intensite_M", "Courant efficace", "A", "A"],
  ["PuissanceS_M", "Puissance <small>(Pw)</small>", "W", "W"],
  ["PowerFactor_M", "Facteur de puissance", "", "phi"],
  ["Energie_M_Soutiree", "Energie active soutirée", "Wh", "Wh"],
  ["Energie_M_Injectee", "Energie active injectée", "Wh", "Wh"],
  ["Tension_T", "Tension efficace", "V", "V"],
  ["Intensite_T", "Courant efficace", "A", "A"],
  ["PuissanceS_T", "Puissance <small>(Pw)</small>", "W", "W"],
  ["PowerFactor_T", "Facteur de puissance", "", "phi"],
  ["Energie_T_Soutiree", "Energie active consommée", "Wh", "Wh"],
  ["Energie_T_Injectee", "Energie active produite", "Wh", "Wh"],
  ["Frequence", "Fréquence", "Hz", "Hz"]
];

// ============================================================================
// Tableaux Enphase
// ============================================================================
const E = [
  ["Tension_M", "Tension efficace", "V", "V"],
  ["Intensite_M", "Courant efficace", "A", "A"],
  ["PuissanceS_M", "Puissance réseau public <small>(Pw)</small>", "W", "W"],
  ["PowerFactor_M", "Facteur de puissance", "", "phi"],
  ["Energie_M_Soutiree", "Energie active soutirée", "Wh", "Wh"],
  ["Energie_M_Injectee", "Energie active injectée", "Wh", "Wh"],
  ["PactProd", "Puissance produite <small>(Pw)</small>", "W", "W"],
  ["PactConso_M", "Puissance consommée <small>(Pw)</small>", "W", "W"],
  ["SessionId", "Session Id", "", "Enph"],
  ["Token_Enphase", "Token", "", "Enph"]
];

// ============================================================================
// Tableau Linky
// ============================================================================
const L = [
  ["EAST", "Energie active soutirée", false, "Wh", 0],
  ["EASF01", "Energie active soutirée Fournisseur,<br>index 01", true, "Wh", 0],
  ["EASF02", "Energie active soutirée Fournisseur,<br>index 02", true, "Wh", 0],
  ["EASF03", "Energie active soutirée Fournisseur,<br>index 03", true, "Wh", 0],
  ["EASF04", "Energie active soutirée Fournisseur,<br>index 04", true, "Wh", 0],
  ["EASF05", "Energie active soutirée Fournisseur,<br>index 05", true, "Wh", 0],
  ["EASF06", "Energie active soutirée Fournisseur,<br>index 06", true, "Wh", 0],
  ["EASF07", "Energie active soutirée Fournisseur,<br>index 07", true, "Wh", 0],
  ["EASF08", "Energie active soutirée Fournisseur,<br>index 08", true, "Wh", 0],
  ["EASF09", "Energie active soutirée Fournisseur,<br>index 09", true, "Wh", 0],
  ["EASF10", "Energie active soutirée Fournisseur,<br>index 10", true, "Wh", 0],
  ["EAIT", "Energie active injectée", false, "Wh", 0],
  ["IRMS1", "Courant efficace, phase 1", true, "A", 0],
  ["IRMS2", "Courant efficace, phase 2", true, "A", 0],
  ["IRMS3", "Courant efficace, phase 3", true, "A", 0],
  ["URMS1", "Tension efficace, phase 1", true, "V", 0],
  ["URMS2", "Tension efficace, phase 2", true, "V", 0],
  ["URMS3", "Tension efficace, phase 3", true, "V", 0],
  ["SINSTS", "Puissance app. instantanée soutirée", false, "VA", 0],
  ["SINSTS1", "Puissance app. instantanée soutirée phase 1", true, "VA", 0],
  ["SINSTS2", "Puissance app. instantanée soutirée phase 2", true, "VA", 0],
  ["SINSTS3", "Puissance app. instantanée soutirée phase 3", true, "VA", 0],
  ["SMAXSN", "Puissance app. max. soutirée n", false, "VA", 1],
  ["SMAXSN1", "Puissance app. max. soutirée n phase 1", true, "VA", 1],
  ["SMAXSN2", "Puissance app. max. soutirée n phase 2", true, "VA", 1],
  ["SMAXSN3", "Puissance app. max. soutirée n phase 3", true, "VA", 1],
  ["SMAXSN-1", "Puissance app. max. soutirée n-1", false, "VA", 1],
  ["SMAXSN1-1", "Puissance app. max. soutirée n-1 phase 1", true, "VA", 1],
  ["SMAXSN2-1", "Puissance app. max. soutirée n-1 phase 2", true, "VA", 1],
  ["SMAXSN3-1", "Puissance app. max. soutirée n-1 phase 3", true, "VA", 1],
  ["SINSTI", "Puissance app. instantanée injectée", false, "VA", 0],
  ["SMAXIN", "Puissance app. max injectée n", false, "VA", 1],
  ["SMAXIN-1", "Puissance app. max injectée n-1", false, "VA", 1],
  ["LTARF", "Option Tarifaire", false, "", 2]
];

// ============================================================================
// Création des tableaux HTML
// ============================================================================

function creerTableauUxIx2() {
  let S = "<table>";
  for (let i = 0; i < M.length; i++) {
    if (i === 0) {
      S += `<tr class="titre"><td id="nomSondeMobile">Maison</td><td></td><td></td></tr>`;
    }
    if (i === 6) {
      S += `<tr class="titre"><td id="nomSondeFixe">Triac</td><td></td><td></td></tr>`;
    }
    S += `<tr class="${M[i][3]}"><td>${M[i][1]}</td><td id="${M[i][0]}" class="ri"></td><td>${M[i][2]}</td></tr>`;
  }
  S += "</table>";
  GH("tableau", S);
}

function creerTableauEnphase() {
  let S = "<table>";
  for (let i = 0; i < E.length; i++) {
    if (i === 0) {
      S += `<tr class="titre"><td id="nomSondeMobile">Maison</td><td></td><td></td></tr>`;
    }
    S += `<tr class="${E[i][3]}"><td>${E[i][1]}</td><td id="${E[i][0]}" class="ri"></td><td>${E[i][2]}</td></tr>`;
  }
  S += "</table>";
  GH("tableauEnphase", S);
}

function creerTableauLinky() {
  let S = "<table>";
  for (let i = 0; i < L.length; i++) {
    S += `<tr id="L${L[i][0]}" style="display:none;" class="${L[i][3]}">
            <td>${L[i][1]}</td>
            <td id="${L[i][0]}" class="ri"></td>
            <td>${L[i][3]}</td>
            <td id="h${L[i][0]}" class="ri"></td>
          </tr>`;
  }
  S += "</table>";
  GH("tableauLinky", S);
}

// ============================================================================
// Récupération données ESP32
// ============================================================================

function LoadDataESP32() {
  const xhttp = new XMLHttpRequest();

  xhttp.onreadystatechange = function () {
    if (this.readyState === 4) {
      if (this.status === 200) {
        const dataESP = this.responseText;
        const Messages = dataESP.split(GS);
        const message = Messages[0].split(RS);

        let S = "<table>";

        let H = parseFloat(message[0]);
        H = (H + (message[0] - H) * 0.6).toFixed(2);
        H = H.replace(".", "h ") + "mn";
        let LaSource=F.Source;
        if (LaSource=='Ext') LaSource="Externe ("+V.Source_data+")<br>" +int2ip(F.RMSextIP);

        const typeESP32 = [
          "Non défini",
          "Wroom seul",
          "Carte 1 relais",
          "Carte 4 relais",
          "Ecran320*240",
          "",
          "",
          "",
          "",
          "",
          "ESP32-ETH01"
        ];

        S+='<tr><td>ESP32 On depuis :</td><td>'+H+'</td></tr>';
        S+='<tr><td>ESP32 modèle :</td><td>'+typeESP32[message[1]]+'</td></tr>';
        S+='<tr><td>Source des mesures :</td><td>'+LaSource+'</td></tr>';
              if (F.ModeReseau<2){
                if (message[1]<10){ //WIFI
                  S+='<tr><td>Niveau WiFi :</td><td>'+message[2]+' dBm</td></tr>';
                  S+="<tr><td>Point d'acc&egrave;s WiFi :</td><td>"+message[3]+'</td></tr>';
                  S+='<tr><td>R&eacute;seau WiFi :</td><td>'+message[6]+'</td></tr>';
                  S+='<tr><td>Canal WiFi :</td><td>'+message[4]+'</td></tr>';
                }
                S+='<tr><td>Adresse MAC ESP32 :</td><td>'+message[5]+'</td></tr>'; 
                let LesIP=message[7].split(US);               
                S+='<tr><td>Adresse IP<small>V4</small> ESP32 :</td><td><a href="http://'+LesIP[0]+'">'+LesIP[0]+'</a></td></tr>';
                S+='<tr><td>Adresse IP .local ESP32 :</td><td><a href="http://'+LesIP[1]+'.local">'+LesIP[1]+'.local</a></td></tr>';
                if (LesIP[2] !=""){
                    S+='<tr><td>Adresse IP<small>V6</small> ESP32 :</td><td><small><a href="http://['+LesIP[2]+']">['+LesIP[2]+']</a></small></td></tr>';
                }
                S+='<tr><td>Adresse passerelle :</td><td>'+message[8]+'</td></tr>';
                S+='<tr><td>Masque du r&eacute;seau :</td><td>'+message[9]+'</td></tr>';
              } else {
                S+="<tr><td>Adresse IP ESP32 (Point d'Accès) :</td><td>192.168.4.1</td></tr>";
              }
              S+='<tr><td>Charge coeur 0 (Lecture Puissance) Min, Moy, Max :</td><td>'+message[10]+' ms</td></tr>';
              S+='<tr><td>Charge coeur 1 (Calcul + Wifi) Min, Moy, Max :</td><td>'+message[11]+' ms</td></tr>';
              S+='<tr><td>Espace mémoire EEPROM utilisé :</td><td>'+message[12]+' %</td></tr>';
              S+='<tr><td>Mémoire RAM libre actuellement :</td><td>'+message[13]+' octet</td></tr>';
              S+='<tr><td>Mémoire RAM libre minimum :</td><td>'+message[14]+' octet</td></tr>';
              S+="<tr><td>Nombre d'interruptions en 15ms du Gradateur (signal Zc) : Filtrés/Brutes :</td><td>"+message[15]+'</td></tr>';
              S+='<tr><td>Synchronisation au Secteur ou asynchrone horloge ESP32</td><td>'+message[16]+'</td></tr>';
              let Stemp=message[17];
              if (message[17]>0) Stemp +='<span class="fsize10">' + message[18] +'</span>';
              S+="<tr><td>Nombre de capteurs de température DS18B20 :</td><td>"+Stemp+'</td></tr>';
              S +='<tr><td style="text-align:center;"><strong>Messages</strong></td><td></td></tr>';
              let message1=Messages[1].split(RS);
              for (let i=1;i<=10;i++){
                S +='<tr><td>'+message1[i]+'</td><td></td></tr>';
              }
              let message2=Messages[2].split(RS);
              if(message2.length>1){
                S +='<tr><td style="text-align:center;"><strong>Note échanges entre routeurs</strong></td><td></td></tr>';
                for (let i=0;i<message2.length-1;i++){
                    let Note=message2[i].split(ES);
                    S +='<tr><td>'+Note[0]+'</td><td>' +Note[1] +'</td></tr>';
                }
              }
              S+='</table>';

        GH("DataESP32", S);
      }

      setTimeout(LoadDataESP32, 5000);
    }
  };

  xhttp.open("GET", "/ajax_dataESP32", true);
  xhttp.send();
}

// ============================================================================
// Fin initialisation
// ============================================================================
function SetParaFixe() {
  LoadParaVar();
}
function SetParaVar() {
  Set_Couleurs();
  LoadData();
  LoadDataESP32();
}
)====";

const char *PageBruteJS2 = R"====(
// ============================================================================
// Récupération des données principales
// ============================================================================


const REFRESH_INTERVAL = 2000; // 2 secondes

async function LoadData() {
    // Affiche l'indicateur de chargement
      GID("LED").style.display = "block";
    

    try {
        const url = `/ajax_dataRMS?idx=${IdxMessage}`;
        const response = await fetch(url);

        // 1. Gérer les erreurs HTTP (e.g., 404, 500)
        if (!response.ok) {
            throw new Error(`Erreur HTTP: ${response.status} - ${response.statusText}`);
        }

        const DuRMS = await response.text();

        // Masque l'indicateur de chargement
        
        GID("LED").style.display = "none";
      

        // --- Début du traitement des données (Logique complexe) ---
        
        // S'assurer que GS et RS sont définis et que DuRMS est une chaîne
        if (typeof GS === 'undefined' || typeof RS === 'undefined') {
             console.error("Variables de séparation (GS, RS) non définies.");
             return; 
        }

        const groupes = DuRMS.split(GS);
        const G0 = groupes[0].split(RS);
        
        // S'assurer que GH est défini
        if (typeof GH !== 'function') {
             console.error("Fonction GH non définie.");
             return; 
        }

        GH('date', G0[0]);
        const Source_data = G0[1];

        // Remplacement des blocs 'if' par un 'switch' pour une meilleure lisibilité
        switch (Source_data) {
            case "NotDef":
                GID('infoNotDef').style.display = "block";
                break;

            case "UxI":
                GID('infoUxI').style.display = "block";
                
                // Assurez-vous que Koul est défini et a la bonne structure
                const colorW = Koul[Coul_W][3];
                const colorVA = Koul[Coul_VA][3];
                const colorGraphe = Koul[Coul_Graphe][1];

                GH('Ueff', `<span style='color:#${colorW};'>_${parseInt(G0[2], 10)}<small> V</small></span>`);
                GH('Ieff', `<span style='color:#${colorVA};'> _${G0[3]}<small> A</small></span>`);
                GH('cosphi', `<span style='color:#${colorGraphe};'> <small>Facteur de puissance : ${G0[4]}</small></span>`);

                // Traitement des données pour le graphique SVG
                const volt = groupes[1].split(RS).map(v => parseFloat(v)); // Conversion en nombres
                const amp = groupes[2].split(RS).map(a => parseFloat(a)); // Conversion en nombres

                // Constantes de style
                const cT = `#${Koul[Coul_Graphe][1]}`;
                const style = `background:linear-gradient(#${Koul[Coul_Graphe][5]},#${Koul[Coul_Graphe][3]},#${Koul[Coul_Graphe][5]});border-color:#${Koul[Coul_Tab][5]};`;
                
                let Vmax = 500;
                let Imax = 500;
                
                // Recherche des valeurs max pour la mise à l'échelle
                for (let i = 0; i < 100; i++) {
                    Vmax = Math.max(Math.abs(volt[i] || 0), Vmax); // Ajout de || 0 pour gérer les valeurs manquantes
                    Imax = Math.max(Math.abs(amp[i] || 0), Imax);   // Ajout de || 0 pour gérer les valeurs manquantes
                }

                // Construction du SVG (Utilisation de template string)
                let S = `<svg height='400' width='1000' style='${style}' >`;
                S += `<line x1='0' y1='400' x2='0' y2='0' style='stroke:${cT};stroke-width:2' />`;
                S += `<line x1='0' y1='197' x2='1000' y2='197' style='stroke:${cT};stroke-width:2' />`;

                // Polyline pour la tension (volt)
                let pointsVolt = '';
                for (let i = 0; i < 100; i++) {
                    // Assurez-vous que volt[i] est un nombre valide
                    const val = volt[i] !== undefined && !isNaN(volt[i]) ? volt[i] : 0; 
                    const Y = 197 - 185 * val / Vmax;
                    const X = 10 * i;
                    pointsVolt += `${X},${Y} `;
                }
                S += `<polyline points='${pointsVolt.trim()}' style='fill:none;stroke:#${Koul[Coul_W][3]};stroke-width:6' />`;

                // Polyline pour le courant (amp)
                let pointsAmp = '';
                for (let i = 0; i < 100; i++) {
                    // Assurez-vous que amp[i] est un nombre valide
                    const val = amp[i] !== undefined && !isNaN(amp[i]) ? amp[i] : 0;
                    const Y = 197 - 185 * val / Imax;
                    const X = 10 * i;
                    pointsAmp += `${X},${Y} `;
                }
                S += `<polyline points='${pointsAmp.trim()}' style='fill:none;stroke:#${Koul[Coul_VA][3]};stroke-width:6' />`;

                S += `</svg>`;
                GH('SVG', S);
                break;

            case "UxIx2":
                GID('infoUxIx2').style.display = "block";
                const G1_UxIx2 = groupes[1].split(RS);
                if (!InitFait) {
                    InitFait = true;
                    if (typeof creerTableauUxIx2 === 'function') creerTableauUxIx2();
                    GH("nomSondeFixe", F.nomSondeFixe);
                    GH("nomSondeMobile", F.nomSondeMobile);
                }
                for (let j = 0; j < M.length; j++) {
                    if (M[j][3] === 'Wh' && typeof LaVal === 'function') {
                        GH(M[j][0], LaVal(G1_UxIx2[j]));
                    } else {
                        GH(M[j][0], G1_UxIx2[j]);
                    }
                }
                break;

            case "Enphase":
                GID('infoEnphase').style.display = "block";
                const G1_Enphase = groupes[1].split(RS);
                if (!InitFait) {
                    InitFait = true;
                    if (typeof creerTableauEnphase === 'function') creerTableauEnphase();
                    GH("nomSondeMobile", F.nomSondeMobile);
                }
                for (let j = 0; j < E.length; j++) {
                    if (E[j][3] === 'Wh' && typeof LaVal === 'function') {
                        GH(E[j][0], LaVal(G1_Enphase[j]));
                    } else {
                        GH(E[j][0], G1_Enphase[j]);
                    }
                }
                break;

            case "SmartG":
                GID('infoSmartG').style.display="block";
                groupes[1] = groupes[1].replaceAll('"','');
                var G1=groupes[1].split(",");
                let Sg="";              
                for (var i=0;i<G1.length;i++){
                      Sg +=G1[i]+"<br>";
                }
                GH('dataSmartG', Sg);
                break;
            case "HomeW":
                GID('infoHomeW').style.display="block";
                groupes[1] = groupes[1].replaceAll('"','');
                var G1=groupes[1].split(",");
                let Sw="";              
                for (var i=0;i<G1.length;i++){
                      Sw +=G1[i]+"<br>";
                }
                GH('dataHomeW', Sw);
                break;
            case "ShellyEm":
            case "ShellyPro":
                
                GID('infoShellyEm').style.display="block";
                
                GID('infoShellyEm').style.display="block";
                groupes[1] = groupes[1].replaceAll('"','');
                var G1=groupes[1].split(",");
                let Ss="";              
                for (var i=0;i<G1.length;i++){
                      Ss +=G1[i]+"<br>";
                }
                GH('dataShellyEm', Ss);
                break;

            case "UxIx3":
                GID('infoUxIx3').style.display="block";
                GH('dataUxIx3', groupes[1]);
                break;
            case "Pmqtt":
                GID('infoPmqtt').style.display="block";
                GH('dataPmqtt', groupes[1]);
                break;
            case "WesV2":
                GID('infoWesV2').style.display="block";
                GH('dataWesV2', groupes[1]);
                break;

            case "Linky":
                GID('infoLinky').style.display = "block";
                if (!InitFait) {
                    InitFait = true;
                    if (typeof creerTableauLinky === 'function') creerTableauLinky();
                }
                
                // Le message Linky peut être très fragmenté, donc on ajoute le nouveau morceau
                MessageLinky += groupes[1];
                
                // Le séparateur de bloc de message Linky est le caractère STX (ASCII 2)
                const blocs = MessageLinky.split(String.fromCharCode(2));
                const lg = blocs.length;
                
                if (lg > 2) {
                    // Si on a au moins un bloc complet + le début du suivant
                    MessageLinky = String.fromCharCode(2) + blocs[lg - 1]; // Garde le début du dernier bloc pour la prochaine fois
                    const dataBlock = blocs[lg - 2];
                    
                    GH('DataLinky', '<pre>' + dataBlock + '</pre>');
                    
                    // Le séparateur de ligne est LF (ASCII 10)
                    const lignes = dataBlock.split(String.fromCharCode(10)); 
                    
                    for (let i = 0; i < lignes.length; i++) {
                        // Le séparateur de colonne est TAB (ASCII 9)
                        const colonnes = lignes[i].split(String.fromCharCode(9)); 
                        
                        if (colonnes[0] === 'DATE' && typeof LaDate === 'function') {
                            GH('dateLinky', LaDate(colonnes[1]));
                        }
                        
                        // Traitement des données Linky à partir du tableau L
                        for (let j = 0; j < L.length; j++) {
                            if (colonnes[0] === L[j][0]) {
                                // Vérifie si la donnée est affichable (pas masquée OU valeur > 0)
                                const isVisible = !L[j][2] || parseInt(colonnes[1]) > 0;
                                
                                if (isVisible) {
                                    GID('L' + L[j][0]).style.display = "table-row";
                                    
                                    switch (L[j][4]) {
                                        case 0: // Valeur simple (ex: index)
                                            GH(L[j][0], typeof LaVal === 'function' ? LaVal(colonnes[1]) : colonnes[1]);
                                            break;
                                        case 1: // Valeur avec horodatage (ex: PMAX)
                                            GH('h' + L[j][0], typeof LaDate === 'function' ? LaDate(colonnes[1]) : colonnes[1]);
                                            GH(L[j][0], typeof LaVal === 'function' ? LaVal(colonnes[2]) : colonnes[2]);
                                            break;
                                        case 2: // Texte
                                            GH('h' + L[j][0], colonnes[1]);
                                            break;
                                    }
                                }
                                break; // Sortir de la boucle L.length
                            }
                        }
                    }
                    GID('LED').style.display = 'none'; // Re-masquer l'indicateur après traitement Linky
                }
                
                // Mettre à jour l'index de message pour la prochaine requête
                IdxMessage = groupes[2];
                break;
                
            default:
                // Si la Source_data est inconnue, ne rien faire
                console.log(`Source de données inconnue : ${Source_data}`);
                break;
        }

    } catch (error) {
        console.error("Erreur lors du chargement des données:", error);
      
        GID("LED").style.display = "none";
        
    } finally {
        // Planifier le prochain appel après un délai, qu'il y ait eu succès ou échec
        setTimeout(() => LoadData(), REFRESH_INTERVAL);
    }
}
      
      function LaDate(d){
          return d.substr(0,1)+' '+d.substr(5,2)+'/'+d.substr(3,2)+'/'+d.substr(1,2)+' '+d.substr(7,2)+'h '+d.substr(9,2)+'mn '+d.substr(11,2)+'s';
      }
      
      function AdaptationSource(){
        if(F.Source=="Ext"){
          GID("donneeDistante").style.display="block";
        }
        
      }
)====";

