//**********************************************
// Page principale  Javascript 
//**********************************************
const char * MainJS1 = R"====(
// NOTA: La variable biSonde est déclarée au chargement du fichier par le serveur pour être dispo immédiatement
let tabPW2sM = [];
let tabPW2sT = [];
let tabActOuvre = [];
let LastPW_M = 0;
let LastPVA_M = 0;
let LastPW_T = 0;
let LastPVA_T = 0;
let LastActOuvre = [];
let PuisMaxS_M = 0;
let PuisMaxI_M = 0;
let PuisMaxS_T = 0;
let PuisMaxI_T = 0;
let initUxIx2 = false;
let TabVal = [];
let TabCoul = [];
let myTimeout;
let myActionTimeout;
let ActionForce = [];
let Pva_valide = false;
let BordsInverse = [".Baccueil"];
let DispAutres = [];


// Fonction pour récupérer les données en temps réel


async function LoadData() {
    const REFRESH_INTERVAL=2000;
    const ERROR_RETRY_INTERVAL=10000;
    
    // Affichage de l'indicateur de chargement
    GID('LED').style.display = 'block';

    try {
        const response = await fetch('/ajax_data');

        // Gérer les erreurs HTTP (e.g., 404, 500)
        if (!response.ok) {
            throw new Error(`Erreur HTTP: ${response.status} ${response.statusText}`);
        }

        const DuRMS = await response.text();
        
        // --- Traitement des données ---
        const groupes = DuRMS.split(GS);
       
        const G0 = groupes[0].split(RS);
        const G1 = groupes[1].split(RS);
        const G2 = groupes.length > 2 ? groupes[2].split(RS) : []; // Initialisation sûre de G2

        // Affectation des variables principales
        GID('date').innerHTML = G0[1];
        Source_data = G0[2]; 

        // Initialisation des variables de l'interface
        if (!initUxIx2) {
            initUxIx2 = true;
        }

        // --- Affectation des valeurs Maison (G1) ---
        // Utilisation d'un tableau pour réduire la répétition
        const elementsMaison = ['PwS_M', 'PwI_M', 'PVAS_M', 'PVAI_M', 'EAJS_M', 'EAJI_M', 'EAS_M', 'EAI_M'];
        for (let i = 0; i < elementsMaison.length; i++) {
            GID(elementsMaison[i]).innerHTML = LaVal(G1[i]);
        }
        
        
        PuisMaxS_M = Math.max(PuisMaxS_M, G1[0]); 
        PuisMaxI_M = Math.max(PuisMaxI_M, G1[1]); 
        GID('PMS_M').innerHTML = LaVal(PuisMaxS_M);
        GID('PMI_M').innerHTML = LaVal(PuisMaxI_M);

     
        LastPW_M = parseFloat(G1[0]) - parseFloat(G1[1]);
        LastPVA_M = parseFloat(G1[2]) - parseFloat(G1[3]);

        // Logique Tarif
        const Tarif = ["NON_DEFINI", "PLEINE", "CREUSE", "BLEU", "BLANC", "ROUGE"];
        const couleur = ["#666" , "#f00", "#0f0", "#00bfff", "#fff", "#f00"];
        const tarif = ["", "H.<br>pleine", "H.<br>creuse", "Tempo<br>Bleu", "Tempo<br>Blanc", "Tempo<br>Rouge"];
        
        let idx_jour = 0;
        // Détermination du tarif du jour (G0[3])
        for (let i = 0; i < Tarif.length; i++) {
            // Utilisation de .includes() pour plus de clarté
            if (G0[3].includes(Tarif[i])) { 
                idx_jour = i;
                break; // Sortir dès que le tarif est trouvé
            }
        }
        GID('couleurTarif_jour').style.backgroundColor = couleur[idx_jour];
        GID('couleurTarif_jour').innerHTML = tarif[idx_jour];
        
        // Détermination du tarif J+1 (G0[4]) - Tempo
        let tempo = parseInt(G0[4], 16) || 0; 
        tempo = Math.floor(tempo / 4); // La valeur est 1 (Bleu), 2 (Blanc), ou 3 (Rouge)
        
        let idx_j1 = -2; // Correspond à l'index "NON_DEFINI" (-2 + 2 = 0)
        let txtJ = "";
        
        if (tempo > 0 && tempo <= 3) {
            // idx = 1, 2, 3 (Bleu, Blanc, Rouge). Dans le tableau couleur/tarif, ils sont à l'index 3, 4, 5
            idx_j1 = tempo + 2; 
            txtJ = "Tempo<br>J+1";
        } else {
             idx_j1 = 0; // Utiliser la couleur NON_DEFINI si la valeur est inattendue ou 0
        }
        
        GID('couleurTarif_J1').style.backgroundColor = couleur[idx_j1];
        GID('couleurTarif_J1').innerHTML = txtJ;
        
        // Validation du PVA
        Pva_valide = (G0[6] === '1'); // Conversion explicite en chaîne pour la comparaison

        // --- Traitement des données Triac/SSR (G2) ---
        // Vérifie si la bi-sonde est active ET si les données du groupe 2 sont présentes
        if (biSonde && groupes.length >= 3 && F.nomSondeFixe !== "") {
            
            // Utilisation d'un tableau pour réduire la répétition (éléments T)
            const elementsTriac = ['PwS_T', 'PwI_T', 'PVAS_T', 'PVAI_T', 'EAJS_T', 'EAJI_T', 'EAS_T', 'EAI_T'];
            for (let i = 0; i < elementsTriac.length; i++) {
                GID(elementsTriac[i]).innerHTML = LaVal(G2[i]);
            }
            
            // Mise à jour des puissances maximales du Triac
            const PwS_T = parseInt(G2[0], 10) ;
            const PwI_T = parseInt(G2[1], 10) ;
            PuisMaxS_T = Math.max(PuisMaxS_T, PwS_T);
            PuisMaxI_T = Math.max(PuisMaxI_T, PwI_T);
            
            GID('PMS_T').innerHTML = LaVal(PuisMaxS_T);
            GID('PMI_T').innerHTML = LaVal(PuisMaxI_T);

            LastPW_T = parseFloat(G2[0]) - parseFloat(G2[1]);
            LastPVA_T = parseFloat(G2[2]) - parseFloat(G2[3]);
        }

        AdaptationSource();

        // Masquage des éléments PVA si non valides
        if (!Pva_valide) {
            const collection = document.getElementsByClassName('VA');
            for (let i = 0; i < collection.length; i++) {
                collection[i].style.display = "none";
            }
        }
        
        // Masquage de l'indicateur de chargement après succès
        GID('LED').style.display = 'none';
        
        // Planification du prochain appel
        setTimeout(LoadData, REFRESH_INTERVAL); 

    } catch (error) {
        console.error("Erreur lors du chargement des données:", error);
        
        // Masquage de l'indicateur de chargement même en cas d'erreur
        GID('LED').style.display = 'none';
        setTimeout(LoadData, ERROR_RETRY_INTERVAL); 
    }
}

// Fonction pour charger l'historique 10 minutes
async function LoadHisto10mn() {
    try {
        const response = await fetch('/ajax_data10mn');

        if (!response.ok) {
            throw new Error(`Erreur HTTP: ${response.status} lors du chargement 10mn.`);
        }

        const retour = await response.text();
        let groupes = retour.split(GS); 
        
        

        // --- Traitement des Puissances (tabPW2sM) ---
        // Utilisation de let pour la réassignation si nécessaire.
        tabPW2sM = groupes[1].split(',').filter(s => s.length > 0); // Filtre pour retirer la dernière entrée vide

        if (biSonde && groupes.length >= 3) {
            tabPW2sT = groupes[2].split(',').filter(s => s.length > 0);
        }
        
        // --- Traitement des Actionneurs (tabActOuvre) ---
        
        // Utilisation de .slice() pour obtenir le reste des groupes (meilleure lisibilité que les shift() répétés)
        const groupesActionneurs = groupes.slice(3); // Commence à l'index 3 (après Pmax, M, T)

        if (groupesActionneurs.length > 0) { 
            
            for (const groupe of groupesActionneurs) { 
                const Act = groupe.split(ES);
                const i = parseInt(Act[0], 10);
                
                // Assurez-vous que 'i' est un index valide avant d'assigner
                if (!isNaN(i) && i >= 0 && Act[1]) {
                    // Utilisation de filter pour retirer le dernier élément vide
                    tabActOuvre[i] = Act[1].split(RS).filter(s => s.length > 0); 
                }
            }
        }
        
        // Passe à l'étape suivante
        await LoadHisto1an();

    } catch (error) {
        console.error("Erreur LoadHisto10mn:", error);
       
    }
}
// Fonction pour charger l'historique 48 heures (avec rafraîchissement périodique)
async function LoadHisto48h() {
    const REFRESH_INTERVAL_48H = 300000; // 5 minutes

    try {
        const response = await fetch('/ajax_histo48h');

        if (!response.ok) {
            throw new Error(`Erreur HTTP: ${response.status} lors du chargement 48h.`);
        }

        const retour = await response.text();
        const groupes = retour.split(GS);
        
        if (groupes.length < 1) {
             throw new Error("Format de données 48h invalide.");
        }

        // --- 1. Maxima (Pmaxi) ---
        const Pmaxi = groupes[0].split(RS);
        PuisMaxS_M = parseInt(Pmaxi[0], 10) ;
        PuisMaxI_M = parseInt(Pmaxi[1], 10) ;
        PuisMaxS_T = parseInt(Pmaxi[2], 10) ;
        PuisMaxI_T = parseInt(Pmaxi[3], 10) ;

        // --- 2. Puissance Mobile (Maison) ---
        let tabPWM = groupes[1].split(',').filter(s => s.length > 0);
        
        
        if (Graphes_Select[2]) Plot('SVG_PW48hM', tabPWM, Koul[Coul_W][3], `Puissance Active ${F.nomSondeMobile} sur 48h en W`, '', '');
        
        
        // --- 3. Puissance Fixe (Triac/SSR) ---
        if (biSonde && groupes.length >= 3 && Graphes_Select[3]) {
            let tabPWT = groupes[2].split(',').filter(s => s.length > 0);
            GID('SVG_PW48hT').style.display = "block";
            Plot('SVG_PW48hT', tabPWT, Koul[Coul_W][3], `Puissance Active ${F.nomSondeFixe} sur 48h en W`, '', '');    
        }
        
        // --- 4. Températures ---
        
        // Les groupes 1 et 2 ont été traités. Les températures commencent au groupe[3]
        const lesTempBlock = groupes[3]; 

        if (lesTempBlock) {
            const lesTemp = lesTempBlock.split("|");
            
            for (let c = 0; c < 4; c++) { 
                if (lesTemp[c] && Graphes_Select[4+c]) {
                    // Les données de température peuvent contenir une dernière valeur non significative, la vérification est maintenue
                    const tabTemperature = lesTemp[c].split(',');
                    const lastTemp = parseFloat(tabTemperature[tabTemperature.length - 1], 10);
                    
                    if (lastTemp > -100 && typeof Plot === 'function' && typeof GID === 'function') { 
                        tabTemperature.pop(); // Retirer la dernière valeur non significative
                        GID('SVG_Temp48h' + c).style.display = "block";
                        let nomT=[F.nomTemperature0,F.nomTemperature1,F.nomTemperature2,F.nomTemperature3];                        
                        Plot('SVG_Temp48h' + c, tabTemperature, Koul[Coul_Temp + c][3], `${nomT[c]} sur 48h `, '', '');
                    }
                }
            }
        }
        
        // --- 5. Ouvertures (Actionneurs) ---
        // Les ouvertures sont dans les groupes suivants (commencent à l'index 4 si groupe[3] était les températures)
        const groupesOuvertures = groupes.slice(4); 
        
        if (groupesOuvertures.length > 0 && Graphes_Select[9]) {
            Plot_ouvertures(groupesOuvertures);
        }
        
    } catch (error) {
        console.error("Erreur LoadHisto48h:", error);
    } finally {
        // Le rafraîchissement se fait toutes les 5 minutes, qu'il y ait eu succès ou échec
        setTimeout(LoadHisto48h, REFRESH_INTERVAL_48H); 
    }
}

// Fonction pour charger l'historique 1 an
async function LoadHisto1an() {
    try {
        const response = await fetch('/ajax_histo1an');

        if (!response.ok) {
            throw new Error(`Erreur HTTP: ${response.status} lors du chargement 1an.`);
        }

        let retour = await response.text();
        retour=JSON.parse(retour);
        const tabWh = retour.Energie1an;
        if (Graphes_Select[11]) Plot('SVG_Wh1an', tabWh, Koul[Coul_Wh][3], 'Energie Active Wh / Jour sur 1an', '', '');
        const tabJWh = retour.EnergieJour;
        let tabJourWh=[];
        let j=-1;
        let lastDay="";
        for (let i=0;i<tabJWh.length;i++){ //On ne garde que les valeurs de fin de journée
            let ValJour=tabJWh[i].split(",");            
            if (ValJour[0] !=lastDay) { //Nouvelle valeur
                j++;
            }
            lastDay=ValJour[0];
            tabJourWh[j]=ValJour.slice(0,5); //Date + Souti+Inject+Conso+Produit          
        }
        if (Graphes_Select[10]) PlotWhJour(tabJourWh);
        // Passe à l'étape suivante
        await LoadHisto48h();

    } catch (error) {
        console.error("Erreur LoadHisto1an:", error);
    }
}
)====";

const char * MainJS2 = R"====(
// Fonction de fin de chargement des paramètres fixes
function SetParaFixe() {
  GH("nomSondeMobile",F.nomSondeMobile);
  Graphes_Nom[0]=F.nomSondeMobile +" / 10 mn";
  Graphes_Nom[2]=F.nomSondeMobile +" / 48 h";
  if (biSonde){
    GH("nomSondeFixe",F.nomSondeFixe); 
    GH("nomSfixePpos",F.nomSfixePpos);
    GH("nomSfixePneg",F.nomSfixePneg);
    Graphes_Nom[1]=F.nomSondeFixe +" / 10 mn";
    Graphes_Nom[3]=F.nomSondeFixe +" / 48 h";
  }
  Graphes_Dispo[4]=F.Source_Temp0!="tempNo";
  Graphes_Dispo[5]=F.Source_Temp1!="tempNo";
  Graphes_Dispo[6]=F.Source_Temp2!="tempNo";
  Graphes_Dispo[7]=F.Source_Temp3!="tempNo";
  Graphes_Nom[4]=F.nomTemperature0;
  Graphes_Nom[5]=F.nomTemperature1;
  Graphes_Nom[6]=F.nomTemperature2;
  Graphes_Nom[7]=F.nomTemperature3;
  GID("B_graph").style.display = "flex";
  GID("date").style.display = "block";
  GID("foot").style.display = "block";
  GID("routeur").style.display = "none";
  LoadParaVar();
}
// Fonction de fin de chargement des paramètres variables
function SetParaVar() {  
  Set_Couleurs();
  setInterval(Refresh_2s, 2000); 
  GID("TabMesures").style.display = "block";
  LoadData();
  LoadHisto10mn();
  EtatActions(0, 0);
  let S = "";
  for (let c = 1; c < V.IP_RMS.length; c++) { 
    S += "<div class='autreRMS'><div>" + V.IP_RMS[c] + "</div><div>" +nomRMS[c] + "</div><div id='autreRid" + c + "' onclick='autreRclick(" + c + ");' style='cursor:pointer;' ></div></div>";
    S += "<div class='autreRif' id='autreRif" + c + "'></div>";
  }
  GH("autresRMS", S);
  
  const IPextDisp = int2ip(F.RMSextIP);
  let IdsxSource = -1;
  
  for (let c = 1; c < V.IP_RMS.length; c++) { 
    if (V.IP_RMS[c] === IPextDisp) IdsxSource = c; 
    autreRaffiche(c);
  }
  
  S = 'Source : ';
  if (F.Source === "Ext") { 
    S += 'ESP distant ' + IPextDisp;
    if (IdsxSource > -1) S += " " + nomRMS[IdsxSource];
    GID("donneeDistante").style.display = "block";
  } else {
    S += 'ESP local';
  }
  GH('source', S);
  
}

// Fonction clic autre routeur
function autreRclick(c) {
  
  
  if (DispAutres[c] == "true") { 
    DispAutres[c] = "false";
  } else {
    DispAutres[c] = "true";
  }
  autreRaffiche(c);
}

// Fonction d'affichage autre routeur
function autreRaffiche(c) {
  let S = "";
  let fleche = "⬆️";
  
  if (DispAutres[c] == "true") { 
    S += "<iframe src='http://" + V.IP_RMS[c] + "' ></iframe>";
  } else {
    fleche = "⬇️";
  }
  GH("autreRif" + c, S);
  GH("autreRid" + c, fleche);
  localStorage.setItem("DispAutres", JSON.stringify(DispAutres));
}

// Fonction de tracé de graphique SVG
var cadrageMax;
function Plot(SVG, Tab, couleur1, titre1, couleur2, titre2) {
  let Vmax = 0;
  const TabY0 = [];
  const TabY1 = [];
  couleur1 = "#" + couleur1;
  couleur2 = "#" + couleur2;
  const dX = 900 / Tab.length;
  const d = new Date();
  let dI = 1;
  let label = 'heure';
  let pixelTic = 72;
  let dTextTic = 4;
  let moduloText = 24;
  const H0 = d.getHours() + d.getMinutes() / 60;
  let H00 = 4 * Math.floor(H0 / 4);
  let X0 = 18 * (H00 - H0);
  let Y0 = 250;
  let X,Y,Y2;
  let Yamp = 230;
 
  let dispVA = false;
  
 
  switch (SVG) {
    case 'SVG_PW48hM':
      break;
    case 'SVG_PW48hT':
      break;
    case 'SVG_Temp48h':
      
      break; 
    case 'SVG_PW2sM':
      label = 'mn';
      pixelTic = 90;
      X0 = 0;
      dTextTic = 1;
      moduloText = -100;
      H00 = 0;
      dI = 2; 
      GID(SVG + '_L').style.color = couleur2; 
      GID(SVG + '_L').style.display = 'block';
      dispVA = GID(SVG + '_C').checked; 
      localStorage.setItem(SVG + '_LS', dispVA);
      break;
    case 'SVG_PW2sT':
      label = 'mn';
      pixelTic = 90;
      X0 = 0;
      dTextTic = 1;
      moduloText = -100;
      H00 = 0;
      dI = 2; 
      GID(SVG + '_L').style.color = couleur2; 
      GID(SVG + '_L').style.display = 'block';
      dispVA = GID(SVG + '_C').checked; 
      localStorage.setItem(SVG + '_LS', dispVA);
      break;
    case 'SVG_Wh1an':
      label = 'Mois';
      pixelTic = dX * 30.4375;
      dTextTic = 1;
      moduloText = 12;
      H00 = d.getMonth();
      X0 = dX * (1 - d.getDate());
      break;
  }
  const c1 = '"' + couleur1 + '"';
  const c2 = '"' + couleur2 + '"';
  const cT = "#" + Koul[Coul_Graphe][1];
  const Mois = ['Jan', 'Fev', 'Mars', 'Avril', 'Mai', 'Juin', 'Juil', 'Ao&ucirc;t', 'Sept', 'Oct', 'Nov', 'Dec'];
  for (let i = 0; i < Tab.length; i++) { 
    if (i % 2 === 0 || dI === 1 || dispVA) { 
      // Assurer la conversion en nombre pour la comparaison
      const val = parseFloat(Tab[i]); 
      Tab[i] = Math.min(val, 10000000);
      Tab[i] = Math.max(Tab[i], -10000000);
      Vmax = Math.max(Math.abs(Tab[i]), Vmax);
    }
  }
  
  let S=PlotCommun(SVG,cT,Vmax,label);
  for (let x = 1000 + X0; x > 100; x = x - pixelTic) { 
    X = x;
    Y2 = Y0 + 6;
    S += "<line x1='" + X + "' y1='" + Y0 + "' x2='" + X + "' y2='" + Y2 + "' style='stroke:" + cT + ";stroke-width:2' />";
    X = X - 8;
    Y2 = Y0 + 22;
    if (SVG === 'SVG_Wh1an') { 
      X = X + 8;
      S += "<text x='" + X + "' y='" + Y2 + "' style='font-size:16px;fill:" + cT + ";'>" + Mois[H00] + "</text>";
    } else {
      S += "<text x='" + X + "' y='" + Y2 + "' style='font-size:16px;fill:" + cT + ";'>" + H00 + "</text>";
    }
    H00 = (H00 - dTextTic + moduloText) % moduloText;
  }
  

  
  if (dI === 2 && Pva_valide && dispVA) { 
    S += "<text x='450' y='40' style='font-size:18px;fill:" + couleur2 + ";'>" + titre2 + "</text>";
    S += "<polyline points='";
    let j = 0;  
    for (let i = 1; i < Tab.length; i = i + dI) { 
      Y = Y0 - Yamp * Tab[i] / cadrageMax;
      X = 100 + dX * i;
      S += X + "," + Y + " ";
      TabY1[j] = parseFloat(Tab[i]);
      j++;
    }
    S += "' style='fill:none;stroke:" + couleur2 + ";stroke-width:2' />";
  }
  
  S += "<text x='450' y='18' style='font-size:18px;fill:" + couleur1 + ";'>" + titre1 + "</text>";
  S += "<polyline points='";
  let j = 0; 
  for (let i = 0; i < Tab.length; i = i + dI) { 
    Y = Y0 - Yamp * Tab[i] / cadrageMax;
    X = 100 + dX * i;
    S += X + "," + Y + " ";
    TabY0[j] = parseFloat(Tab[i]);
    j++;
  }
  S += "' style='fill:none;stroke:" + couleur1 + ";stroke-width:2' />";
  S += "</svg>";
  
  GID(SVG).innerHTML = S;
  TabVal["S_" + SVG] = [TabY0, TabY1];
  TabCoul["S_" + SVG] = [couleur1, couleur2];
}

function PlotWhJour(tabWh){
  if (tabWh.length>0){
    let couleur1="#"+ Koul[Coul_W][3];
    let couleur2="#"+ Koul[Coul_VA][3];
    const cT = "#" + Koul[Coul_Graphe][1];
    let cWh1=  Koul[Coul_Wh][3];
    let cWh2="#" + cWh1.substring(4,6) + cWh1.substring(2,4) + cWh1.substring(0,2) ;//couleur injectée
    cWh1="#" + cWh1;//Couleur soutirée
    let Y0 = 250,Yamp = 230;
    let X,Y;
    let Vmax=1;
    for (let i = 0; i < tabWh.length; i++) { 
      for (let j=1;j<=4;j++) {
        // Assurer la conversion en nombre pour la comparaison
        let val = parseInt(tabWh[i][j]); 
        val = Number.isNaN(val) ? 0 : val;
        if (val>=0){
          Vmax=Math.max(Vmax,val);
        } else {
          Vmax=Math.max(Vmax,-val);
        }
      }
    }

    let S=PlotCommun("SVG_WhJour",cT,Vmax,"Date");
    const dX = 900 / tabWh.length;
    const step=Math.floor(tabWh.length/10);
    const dX2=0.9*dX;
    let Sconso="",Sprod="";
    for (let i = 0; i < tabWh.length; i++) { 
      let  H1=Yamp * tabWh[i][1] / cadrageMax; //Soutire
      Y = Y0 - H1;
      X = 100 + dX * i;
      S +="<rect width='" +dX2 +"' height='"+H1 + "' x='"+ X +"' y='"+Y+"'  fill='"+cWh1+"A0' />";
      let  H2=Yamp * tabWh[i][2] / cadrageMax; //Injecté
      S +="<rect width='" +dX2 +"' height='"+H2 + "' x='"+ X +"' y='"+Y0+"'  fill='"+cWh2+"A0' />";
      if (Sconso!="" || tabWh[i][3] !=0){ //Données seconde sonde
        let Y3=Y0-Yamp * tabWh[i][3] / cadrageMax;
        Sconso += X+"," + Y3+" "; 
      }
      if (Sprod!="" || tabWh[i][4] !=0){ //Données seconde sonde
        let Y4=Y0-Yamp * tabWh[i][4] / cadrageMax;
        Sprod += X+"," + Y4+" "; 
      }
      if (i%step==0){
        let Y5=Y0+50;
        let X2=X+12;
        S += `<text x='${X2}' y='${Y5}'  transform='rotate(-90,${X2},${Y5})' style='font-size:12px;fill:${cT};'>${tabWh[i][0]}</text>`; //La Date
      }
    }
    S += `<text x='110' y='18' style='font-size:18px;fill:${cWh1};'>Energie Soutirée </text>`;
    S += `<text x='550' y='18' style='font-size:18px;fill:${cWh2};'>Energie Injectée</text>`;
    if (Sconso!=""){
      S += `<polyline points='${Sconso}' style='fill:none;stroke:${couleur1};stroke-width:2' />`;    
      S += `<text x='300' y='18' style='font-size:18px;fill:${couleur1};'>${F.nomSondeFixe} / ${F.nomSfixePpos}</text>`;
    }
    if (Sprod!=""){
      S += `<polyline points='${Sprod}' style='fill:none;stroke:${couleur2};stroke-width:2' />`;
      S += `<text x='690' y='18' style='font-size:18px;fill:${couleur2};'>${F.nomSondeFixe} / ${F.nomSfixePneg}</text>`;
    }
    S += "</svg>";
    GID("SVG_WhJour").innerHTML = S;
    let TabWh = tabWh[0].map((_, c) => tabWh.map(ligne => ligne[c])); //Transposition matrice
    TabVal["S_SVG_WhJour"] =TabWh;
    TabCoul["S_SVG_WhJour" ] = [cT,cWh1, cWh2,couleur1, couleur2];
  }
}
// 
function PlotCommun(SVG,cT,Vmax,label){
  let Y0 = 250,Yamp = 230;
  let dy = 2;
  let Y2;
  cadrageMax = 1;
  Vmax=Math.min(Vmax,10000000);
  let cadrage1 = 1000000;
  const cadrage2 = [10, 8, 5, 4, 2, 1];
  
  for (let m = 0; m < 7; m++) { 
    for (let i = 0; i < cadrage2.length; i++) { 
      X = cadrage1 * cadrage2[i];
      if ((Vmax) <= X) cadrageMax = X;
    }
    cadrage1 = cadrage1 / 10;
  }

 
  const style = 'background:linear-gradient( #' + Koul[Coul_Graphe][5] + ',#' + Koul[Coul_Graphe][3] + ',#' + Koul[Coul_Graphe][5] + ');border-color:#' + Koul[Coul_Tab][5] + ';';
  
  let S = "<svg viewbox='0 0 1030 500' style='" + style + "' height='500' width='100%' id='S_" + SVG + "' onmousemove ='DispVal(this,event);' >";
  S += "<line x1='100' y1='20' x2='100' y2='480' style='stroke:" + cT + ";stroke-width:2' />"; //Axe vertical
  S += "<line x1='100' y1='" + Y0 + "' x2='1000' y2='" + Y0 + "' style='stroke:" + cT + ";stroke-width:2' />"; //Axe horizontal

 for (let y = -10; y <= 10; y = y + dy) { // pointillé horizont
    Y2 = Y0 - Yamp * y / 10;
    if (Y2 <= 480) {
      S += "<line x1='100' y1='" + Y2 + "' x2='1000' y2='" + Y2 + "' style='stroke:" + cT + ";stroke-width:1;stroke-dasharray:2 10;' />";
      Y2 = Y2 + 7;
      let T = cadrageMax * y / 10; T = T.toString(); 
      X = 90 - 9 * T.length;
      S += "<text x='" + X + "' y='" + Y2 + "' style='font-size:16px;fill:" + cT + ";'>" + T + "</text>";
    }
  }
  Y2 = Y0 - 3;
  S += "<text x='980' y='" + Y2 + "' style='font-size:14px;fill:" + cT + ";'>" + label + "</text>";  //Label axe X
  return S;
}
// Fonction d'affichage des valeurs au survol
function DispVal(t, evt) {
  const ClientRect = t.getBoundingClientRect();
  const largeur_svg = ClientRect.right - ClientRect.left - 4; 
  let x = Math.round(evt.clientX - ClientRect.left - 2); 
  x = x * 1030 / largeur_svg;
  const p = Math.floor((x - 100) * TabVal[t.id][0].length / 900);
  
  if (p >= 0 && p < TabVal[t.id][0].length) {
    let S = "";
    for (let j = 0; j < TabVal[t.id].length; j++) { 
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
    
    if (myTimeout !== null) clearTimeout(myTimeout); 
    myTimeout = setTimeout(stopAffiche, 5000); // Référence de fonction
  }
}

// Fonction pour masquer l'affichage
function stopAffiche() {
  GID("info").style.display = "none";
}

// Fonction de tracé des ouvertures
function Plot_ouvertures(Gr) {
  GID("SVG_Ouvertures").style.display = "block";
  const d = new Date();
  const label = 'heure';
  const pixelTic = 72;
  const dTextTic = 4;
  const moduloText = 24;
  const H0 = d.getHours() + d.getMinutes() / 60;
  let H00 = 4 * Math.floor(H0 / 4);
  let X0 = 18 * (H00 - H0);
  const Hmax = 50 + 150 * Gr.length;
  let Y0 = Hmax - 50;
  let X,Y,Y2,Y00;
  const Couls = ["#" + Koul[Coul_Ouvre][3], "#" + Koul[Coul_Ouvre + 1][3], "#" + Koul[Coul_Ouvre + 2][3], "#" + Koul[Coul_Ouvre + 3][3]];
  const LesVals = [];
  const LesCouls = [];
  const cT = "#" + Koul[Coul_Graphe][1];
  const style = 'background:linear-gradient(#' + Koul[Coul_Graphe][5] + ',#' + Koul[Coul_Graphe][3] + ',#' + Koul[Coul_Graphe][5] + ');border-color:#' + Koul[Coul_Tab][5] + ';';
  
  let S = "<svg viewbox='0 0 1030 " + Hmax + "' height='" + Hmax + "' style='" + style + "' width='100%' id='S_Ouvertures' onmousemove ='DispVal(this,event);'>";
  S += "<line x1='100' y1='" + Y0 + "' x2='1000' y2='" + Y0 + "' style='stroke:" + cT + ";stroke-width:2' />";
  
  for (let x = 1000 + X0; x > 100; x = x - pixelTic) { 
     X = x;
     Y2 = Y0 + 6; 
    S += "<line x1='" + X + "' y1='" + Y0 + "' x2='" + X + "' y2='" + Y2 + "' style='stroke:" + cT + ";stroke-width:2' />";
    X = X - 8;
    Y2 = Y0 + 22;
    S += "<text x='" + X + "' y='" + Y2 + "' style='font-size:16px;fill:" + cT + ";'>" + H00 + "</text>";
    H00 = (H00 - dTextTic + moduloText) % moduloText;
  }
  
  Y2 = Y0 - 3;
  S += "<text x='980' y='" + Y2 + "' style='font-size:14px;fill:" + cT + ";'>" + label + "</text>";
  
  for (let i = 0; i < Gr.length; i++) { 
    const tableau = Gr[i].split(RS);
    Y00 = (i + 1) * 150;
    Y2 = Y00 - 110;
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
    
    for (let j = 0; j < tableau.length; j++) { 
       Y = Y00 - tableau[j];
       X = 100 + 1.5 * j;
      S += X + "," + Y + " ";
    }
    S += "' style='fill:none;stroke:" + Couls[i % 4] + ";stroke-width:2' />";

    LesVals.push(tableau);
    LesCouls.push(Couls[i % 4]);
  }
  S += "</svg>";
  TabVal["S_Ouvertures"] = LesVals;
  TabCoul["S_Ouvertures"] = LesCouls;
  GID("SVG_Ouvertures").innerHTML = S;
}

// Fonction de tracé des ouvertures (2s)
function Plot_ouvertures_2s() {
  const label = 'mn';
  const pixelTic = 90;
  let X0 = 0;
  const dTextTic = 1;
  const moduloText = -100;
  let H00 = 0; 
  const Hmax = 50 + 150 * nomActions[0].length;
  let Y0 = Hmax - 50;
  let X,Y,Y2,Y00;
  const Couls = ["#" + Koul[Coul_Ouvre][3], "#" + Koul[Coul_Ouvre + 1][3], "#" + Koul[Coul_Ouvre + 2][3], "#" + Koul[Coul_Ouvre + 3][3]];
  const LesVals = [];
  const LesCouls = [];
  const cT = "#" + Koul[Coul_Graphe][1];
  const style = 'background:linear-gradient(#' + Koul[Coul_Graphe][5] + ',#' + Koul[Coul_Graphe][3] + ',#' + Koul[Coul_Graphe][5] + ');border-color:#' + Koul[Coul_Tab][5] + ';';
  
  let S = "<svg viewbox='0 0 1030 " + Hmax + "' height='" + Hmax + "' style='" + style + "' width='100%' id='S_Ouvertures_2s' onmousemove ='DispVal(this,event);'>";
  S += "<line x1='100' y1='" + Y0 + "' x2='1000' y2='" + Y0 + "' style='stroke:" + cT + ";stroke-width:2' />";
  
  for (let x = 1000 + X0; x > 100; x = x - pixelTic) { 
    X = x;
    Y2 = Y0 + 6; 
    S += "<line x1='" + X + "' y1='" + Y0 + "' x2='" + X + "' y2='" + Y2 + "' style='stroke:" + cT + ";stroke-width:2' />";
    X = X - 8;
    Y2 = Y0 + 22;
    S += "<text x='" + X + "' y='" + Y2 + "' style='font-size:16px;fill:" + cT + ";'>" + H00 + "</text>";
    H00 = (H00 - dTextTic + moduloText) % moduloText;
  }
  
  Y2 = Y0 - 3;
  S += "<text x='980' y='" + Y2 + "' style='font-size:14px;fill:" + cT + ";'>" + label + "</text>";
  
  for (let i = 0; i < nomActions[0].length; i++) { 
    GID("SVG_Ouvertures_2s").style.display = "block";
    const idxAction = parseInt(nomActions[0][i][0], 10);
    
    // Vérification de la taille avant d'appeler shift
    if (tabActOuvre[idxAction] && tabActOuvre[idxAction].length > 0) {
        tabActOuvre[idxAction].shift(); 
    } else {
        // Initialiser si nécessaire
        tabActOuvre[idxAction] = []; 
    }
    
    // S'assurer que LastActOuvre[idxAction] existe
    const lastOuvre = LastActOuvre[idxAction] !== undefined ? LastActOuvre[idxAction] : 0;
    tabActOuvre[idxAction].push(lastOuvre);
    
    Y00 = (i + 1) * 150;
    Y2 = Y00 - 110;
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
    
    if (tabActOuvre[idxAction] && tabActOuvre[idxAction].length > 0) {
      S += "<polyline points='";
      for (let j = 0; j < tabActOuvre[idxAction].length; j++) { 
        Y = Y00 - tabActOuvre[idxAction][j];
        X = 100 + 3 * j;
        S += X + "," + Y + " ";
      }
      S += "' style='fill:none;stroke:" + Couls[i % 4] + ";stroke-width:2' />";
      LesVals.push(tabActOuvre[idxAction]);
      LesCouls.push(Couls[i % 4]);
    }
  }
  S += "</svg>";
  TabVal["S_Ouvertures_2s"] = LesVals;
  TabCoul["S_Ouvertures_2s"] = LesCouls;
  GID("SVG_Ouvertures_2s").innerHTML = S;
}



)====";

const char * MainJS3 = R"====(



const REFRESH_INTERVAL_ACTIONS = 3500; // 3.5 secondes


async function EtatActions(Force, NumAction) {
    
    // 1. Annuler le timeout précédent pour éviter les appels doubles
    if (myActionTimeout !== null) {
        clearTimeout(myActionTimeout);
        myActionTimeout = null; // Réinitialiser le timeout
    }

    try {
        const url = `/ajax_etatActions?Force=${Force}&NumAction=${NumAction}`;
        const response = await fetch(url);

        if (!response.ok) {
            throw new Error(`Erreur HTTP: ${response.status} lors du chargement des actions.`);
        }

        const retour = await response.text();
        
        if (typeof GS === 'undefined' || typeof RS === 'undefined') {
             throw new Error("Séparateurs GS ou RS non définis.");
        }
        
        const message = retour.split(GS);
        const LesTemp = message[0].split("|");
        Source_data = message[1]; 

        // --- 2. Traitement des Températures ---
        let T_html = "";
        for (let c = 0; c < Math.min(4, LesTemp.length); c++) { 
            const tempValue = parseFloat(LesTemp[c]);

            if (tempValue > -100) {
                const Temper = tempValue.toFixed(1);
                const nom=F[`nomTemperature${c}`];
                T_html += `<div class='item_temp_nom ce'>${nom}</div><div class='item_temp_val ce'>${Temper}°C</div>`;
            }
        }
        
        // --- 3. Traitement des Actions et Forçage ---
        let S_html = "";
        const nombreActions = parseInt(message[3], 10) || 0; // Ajout d'une valeur par défaut
        
        if (nombreActions > 0) {
            
            // Réinitialisation du tableau ActionForce
            ActionForce.length = 0; 
            
            for (let i = 0; i < nombreActions; i++) { 
                // data est à partir de message[i + 4]
                const data = message[i + 4].split(RS); 
                
                if (data.length < 5) continue; // Sauter les lignes mal formatées

                // Stockage du forçage actuel
                ActionForce[i] = data[3];
                let ouvre = 0;
                
                S_html += `<div class='item_val ce'>${data[1]}</div>`; // Nom de l'action

                // Logique d'ouverture (On/Off vs Pourcentage)
                if (data[2] === "On" || data[2] === "Off") { 
                    S_html += `<div class='item_val ce'>${data[2]}</div>`;
                    if (data[2] === "On") ouvre = 100;
                } else {
                    // Calcul de la jauge pour l'ouverture en pourcentage
                    const pourcentage = parseFloat(data[2]);
                    const W = 1 + 1.99 * pourcentage;
                    
                    S_html += `<div class='item_val ce'>
                        <div class='jaugeBack item_val'>
                            <div class='jauge' style='width:${W}px'></div>
                            <div class='ce w100'>${data[2]}%</div>
                        </div>
                    </div>`;
                    ouvre = parseInt(data[2], 10);
                }
                
                // Stockage de l'ouverture (LastActOuvre doit être un tableau global)
                LastActOuvre[parseInt(data[0], 10)] = ouvre; 
                S_html += `<div class='ce item_val'>${Hdeci2Hmn(data[4])}</div>`;
               

                // Logique des boutons de Forçage
                const forceValue = parseInt(ActionForce[i], 10) || 0;
                const stOn = (forceValue > 0) ? "style='background-color:#f66;'" : "";
                const stOff = (forceValue < 0) ? "style='background-color:#f66;'" : "";
                const minText = (forceValue === 0) ? "&nbsp;&nbsp;" : `${Math.abs(forceValue)} min`;
                
                S_html += `
                    <div class='item_F ce'><input type='button' onclick='EtatActions(1, ${data[0]});' value='On' ${stOn}></div>
                    <div class='item_F ce'><small>${minText}</small></div>
                    <div class='item_F ce'><input type='button' onclick='EtatActions(-1, ${data[0]});' value='Off' ${stOff}></div>
                `;
            }
        }
        
        // --- 4. Affichage Final ---
        S_html = S_html + T_html;
        
        if (S_html !== "") { 
            const header = "<div class='item_Act ce'>Etat Action(s)</div><div class='item_H ce'>H<div class='fsize10'>ouverture équivalente</div></div><div class='item_Force ce'> Forçage</div>";
                GH("etatActions", header + S_html);
                setBoColorQuery("th", "#" + Koul[Coul_Tab][5]); 
                setBoColorQuery("td", "#" + Koul[Coul_Tab][5]);
                GID("etatActions").style.display = "grid";
          
        }

    } catch (error) {
        console.error("Erreur EtatActions:", error);
    } finally {
        // 5. Planification du prochain appel (polling)
        myActionTimeout = setTimeout(() => { EtatActions(0, 0); }, REFRESH_INTERVAL_ACTIONS); 
    }
}

// Fonction de rafraîchissement 2 secondes
function Refresh_2s() {
  if (tabPW2sM.length > 0) {
    tabPW2sM.shift(); 
    tabPW2sM.shift(); 
    tabPW2sM.push(LastPW_M);
    tabPW2sM.push(LastPVA_M);
    
    // Ajout d'une vérification pour biSonde avant de manipuler tabPW2sT
    if (biSonde) {
        tabPW2sT.shift(); 
        tabPW2sT.shift(); 
        tabPW2sT.push(LastPW_T);
        tabPW2sT.push(LastPVA_T);
    }
    
    GH("nomSondeMobile", F.nomSondeMobile);
    if (biSonde) GH("nomSondeFixe", F.nomSondeFixe)
    
    let Nom_simul = F.nomSondeMobile; 
    if (Source_data === "NotDef") Nom_simul = "(Puissance simulée. Source inconnue)"; 
    
    if (Graphes_Select[0]) Plot('SVG_PW2sM', tabPW2sM, Koul[Coul_W][3], 'Puissance Active ' + Nom_simul + ' sur 10 mn en W', Koul[Coul_VA][3], 'Puissance Apparente sur 10 mn en VA');
    
    if (biSonde && Graphes_Select[1]) {
      if (Source_data !== "NotDef") Nom_simul = GID("nomSondeFixe").innerHTML; 
      GID('SVG_PW2sT').style.display = "block";
      Plot('SVG_PW2sT', tabPW2sT, Koul[Coul_W][3], 'Puissance Active ' + Nom_simul + ' sur 10 mn en W', Koul[Coul_VA][3], 'Puissance Apparente sur 10 mn en VA');
    }
    
    if (tabActOuvre.length > 0 && Graphes_Select[8]) Plot_ouvertures_2s();
  }
}



// Fonction pour forcer une action
function Force(NumAction, Force) {
  if (myActionTimeout !== null) clearTimeout(myActionTimeout); 
  EtatActions(Force, NumAction);
}

// Fonction d'adaptation de l'affichage en fonction de la source
function AdaptationSource() {
  let col4="auto";
  let col5="auto";
  if (biSonde) {
    if ( F.nomSfixePpos=="") { 
      GID('nomSfixePpos').innerHTML = '';
      GID('nomSfixePpos').style.padding = '0px';
      GID('PwS_T').innerHTML = '';
      GID('PVAS_T').innerHTML = '';
      GID('EAJS_T').innerHTML = '';
      GID('EAS_T').innerHTML = '';
      GID('PMS_T').innerHTML = '';
      col4="0px";  
    }
    if ( F.nomSfixePneg=="") { 
      GID('nomSfixePneg').innerHTML = '';
      GID('nomSfixePneg').style.padding = '0px';
      GID('PwI_T').innerHTML = '';
      GID('PVAI_T').innerHTML = '';
      GID('EAJI_T').innerHTML = '';
      GID('EAI_T').innerHTML = '';
      GID('PMI_T').innerHTML = '';
      col5="0px";  
    }

    const collection = document.getElementsByClassName('grid-container2M');
    for (let i = 0; i < collection.length; i++) { 
      collection[i].style.gridTemplateColumns = "auto auto auto "+col4 +" " + col5 +" auto";
    }
  }
}


// Fonction d'initialisation
var Graphes_Nom=["Pw / 10 mn","Pw T","Pw / 48h","Pw 48h T","Temperature 0","Temperature 1","Temperature 2","Temperature 3","Ouvertures / 10mn","Ouvertures / 48h","Wh Jour","Wh 1an"];
var Graphes_Dispo=[true,true,true,true,true,true,true,true,true,true,true,true]; //Il y a des données
var Graphes_Select=[true,true,true,true,true,true,true,true,true,true,true,true]; //Affichage choisi par la personne
var Graphes_Ordre=[0,1,2,3,4,5,6,7,8,9,10,11]; //12 graphiques différents
function Init() {
  SetHautBas();
  
  let VA_M = "true";
  let VA_T = "true";
  
  for (let i = 0; i < 8; i++) { 
    DispAutres[i] = "false"; // Initialiser avec 'false' string pour correspondre à localStorage
  }
  if (localStorage.getItem("DispAutres") !== null) DispAutres= JSON.parse(localStorage.getItem("DispAutres"));
  
  if (localStorage.getItem("SVG_PW2sM_LS") !== null) { 
    VA_M = localStorage.getItem("SVG_PW2sM_LS");
    VA_T = localStorage.getItem("SVG_PW2sT_LS"); 
  }
  
  
  let Graphes=[
            `<p id="SVG_PW2sM"></p>
            <div class="choixG">
              <div class="choix" id="SVG_PW2sM_L">
                <label for="SVG_PW2sM_C">VA</label>
                <input type="checkbox" id="SVG_PW2sM_C">
              </div>
            </div>`,

            `<p id="SVG_PW2sT"></p>
            <div class="choixG">
              <div class="choix" id="SVG_PW2sT_L">
                <label for="SVG_PW2sT_C">VA</label>
                <input type="checkbox" id="SVG_PW2sT_C">
              </div>
            </div>`,

            `<p id="SVG_PW48hM"></p>`,
            `<p id="SVG_PW48hT"></p>`,

            `<p class="SVG_Temp48h" id="SVG_Temp48h0"></p>`,
            `<p class="SVG_Temp48h" id="SVG_Temp48h1"></p>`,
            `<p class="SVG_Temp48h" id="SVG_Temp48h2"></p>`,
            `<p class="SVG_Temp48h" id="SVG_Temp48h3"></p>`,

            `<p id="SVG_Ouvertures_2s"></p>`,
            `<p id="SVG_Ouvertures"></p>`,
            `<p id="SVG_WhJour"></p>`,
            `<p id="SVG_Wh1an"></p>`,
  ];
  
  
  let Ordre_g= JSON.parse(localStorage.getItem("TableauGraphiques"));
  if (Ordre_g!=null) {
      Graphes_Ordre=Ordre_g;
  }
  let Ordre_gS= JSON.parse(localStorage.getItem("TableauGraphiquesSelected"));
  if (Ordre_gS!=null) {
      Graphes_Select= Ordre_gS;
  }
  let G="";
  for (let i=0;i<Graphes_Ordre.length;i++){
    G +=Graphes[Graphes_Ordre[i]]; //Zones positionnes pour chaque Graphe
  }
  GH("LesGraphes",G);
  Graphes_Dispo[1]=biSonde;
  Graphes_Dispo[3]=biSonde;
  
  let S = ''; 
  if (biSonde) {
    S = '<div class="grid-container2M">';
    S += '<div class="item1 ce" id="nomSondeMobile">Maison</div>';
    S += '<div class="item2 ce" id="nomSondeFixe">Triac/SSR</div>';
    S += '<div class="item3 ce" id="couleurTarif_jour">Tarif</div>';

    S += '<div class="item4 ce">Soutirée</div>';
    S += '<div class="item5 ce">Injectée</div>';
    S += '<div class="item6 ce" id="nomSfixePpos">Conso.</div>';
    S += '<div class="item7 ce" id="nomSfixePneg">Produite</div>';
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
    S = '<div class="grid-container1">';
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
  LoadParaFixe();
  

  GID("SVG_PW2sM_C").checked = (VA_M === "true") ? true : false;
  GID("SVG_PW2sT_C").checked = (VA_T === "true") ? true : false;
  
  
}

function ListeGraph(){
  let Gnom=`<div>Choix des graphiques</div><div id="B_Graph_in">`;
  for (let i=0;i<Graphes_Ordre.length;i++){
    let Ord=Graphes_Ordre[i];
    if (Graphes_Dispo[Ord]){
      let chk="";
      if (Graphes_Select[Ord]) chk="checked";   
        Gnom +=`<input type='checkbox' onchange="Gr_Select();" id="GraphCheck${Ord}" ${chk}><div>${Graphes_Nom[Ord]}</div><div onclick="decalG(${i},-1);">⬆️</div><div onclick="decalG(${i},1);">⬇️</div>`;
    }
  } 
  Gnom +=`</div><input type="button" value="Valider" onclick="Gr_Select();location.reload();" >`;
  GH("B_Graph",Gnom);
  GID("B_Graph").style= 'background:linear-gradient( #' + Koul[Coul_Graphe][5] + ',#' + Koul[Coul_Graphe][3] + ',#' + Koul[Coul_Graphe][5] + ');';
  GID("B_Graph").style.display="block";
}
function decalG(i,s){
  let j=parseInt(i);
  let Modulo = Graphes_Ordre.length;
  let dispo=false;
  while (!dispo){
      if (s<0) {
        j--;
      } else {
        j++;
      }
      j=(j+Modulo)%Modulo;
      dispo=Graphes_Dispo[j];
  }
  let k=Graphes_Ordre[i];
  Graphes_Ordre[i]=Graphes_Ordre[j];
  Graphes_Ordre[j]=k;
  localStorage.setItem("TableauGraphiques", JSON.stringify(Graphes_Ordre));
  Gr_Select();
  ListeGraph();
}
function Gr_Select(){
  for (let i=0;i<Graphes_Ordre.length;i++){
    let Ord=Graphes_Ordre[i];
    if (Graphes_Dispo[Ord]){      
      Graphes_Select[Ord]= GID("GraphCheck" + Ord).checked ? true : false;
    }
  }
  localStorage.setItem("TableauGraphiquesSelected",JSON.stringify(Graphes_Select));
}
)====";
// icône panneaux solaire et soleil (Merci michy)
const char * Favicon = R"====(
<svg xmlns = "http://www.w3.org/2000/svg" width = "64" height = "64" viewBox = "13 18 46 44" >
<rect width="64" height="64" x="0" y="0"  fill="SpringGreen" />
<path d="m16 36h19l16 20h-20z" fill="#92d3f5"></path>
<circle cx="48" cy="28" fill="#fcea2b" r="8"></circle>
<g fill="none" stroke="#000" stroke-linecap="round" stroke-linejoin="round" stroke-width="2">
  <path d="m16 36h19l16 20h-20z"></path><path d="m17 43v13h10"></path>
  <path d="m24 46h19"></path><path d="m26 36 16 20"></path>
  <circle cx="48" cy="28" r="8"></circle>
</g>
</svg >
)====";
// icône panneaux solaire et soleil 192pixels
const char * Favicon192 = R"====(
<svg xmlns = "http://www.w3.org/2000/svg" width = "192" height = "192" viewBox = "13 18 46 44" >
<rect width="64" height="64" x="0" y="0"  fill="SpringGreen" />
<path d="m16 36h19l16 20h-20z" fill="#92d3f5"></path>
<circle cx="48" cy="28" fill="#fcea2b" r="8"></circle>
<g fill="none" stroke="#000" stroke-linecap="round" stroke-linejoin="round" stroke-width="2">
  <path d="m16 36h19l16 20h-20z"></path><path d="m17 43v13h10"></path>
  <path d="m24 46h19"></path><path d="m26 36 16 20"></path>
  <circle cx="48" cy="28" r="8"></circle>
</g>
</svg >
)====";
// Manifest pour Android
const char * Manifest = R"====(
{
  "name": "Routeur F1ATB",
  "short_name": "Routeur",
  "start_url": "/",
  "display": "standalone",
  "icons": [
    {
      "src": "/favicon192.ico",
      "sizes": "192x192",
      "type": "image/svg+xml"
    },
    {
      "src": "/favicon.ico",
      "sizes": "64x64",
      "type": "image/svg+xml"
    }
  ]
}
)====";
