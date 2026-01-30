//Paramètres du routeur et fonctions générales pour toutes les pages.
const char *ParaCommunJS = R"====(
  var ES=String.fromCharCode(27); //Escape Separator
  var FS=String.fromCharCode(28); //File Separator
  var GS=String.fromCharCode(29); //Group Separator
  var RS=String.fromCharCode(30); //Record Separator
  var US=String.fromCharCode(31); //Unit Separator
  var nb_ESP = 0;
  var nomRMS=[];
  var nomActions=[];
  var F; //Objet Paramètres Fixes
  var V; //Objet Paramètres Variables
  function LoadParaFixe(){
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
             F=JSON.parse(this.responseText);
             GH("nom_R",F.nomRouteur); //Commun à toutes les pages
             let V=parseInt(F.VersionStocke)/100;
             GH("version",`${V.toFixed(2)} | Source : ${F.Source}`);
             document.title=`${F.nomRouteur} - ${document.title}`;
             SetParaFixe();
           }         
        };
        xhttp.open('GET', '/ParaFixe', true);
        xhttp.send();
  }  
  function LoadParaVar() {
    const xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
             V=JSON.parse(this.responseText);             
             AdaptationSource();  
             V.IP_RMS[0] = V.localIP; //Valeur à jour qui a pu changer par DHCP
             nb_ESP=V.RMS_NomEtat.length;
             for (let c=0;c<nb_ESP;c++){
              V.RMS_NomEtat[c]=decodeURIComponent(V.RMS_NomEtat[c]);
              let ip_nom=V.RMS_NomEtat[c].split(US); 
              nomRMS[c]=ip_nom[0];
              //ip_nom[1] les températures n'est plus utilisé
              let lesNomsActions=ip_nom[2].split(FS);
              nomActions[c]=[]; 
              for (let i=0;i<lesNomsActions.length -1;i++){
                  let Nact=lesNomsActions[i].split(ES);
                  nomActions[c][i]=Nact;
              }
             }
             nomRMS[0] = nomRMS[0] +" (local)";
             SetParaVar();
          }         
        };
        xhttp.open('GET', '/ParaVar', true);
        xhttp.send();
  }
  function GID(id) { return document.getElementById(id); }
  function GH(id, T) {
    if ( GID(id)){
     GID(id).innerHTML = T; }
    }
  function GV(id, T) { GID(id).value = T; }
  
  function int2ip (V) {
    let ipInt=parseInt(V);
    return ( (ipInt>>>24) +'.' + (ipInt>>16 & 255) +'.' + (ipInt>>8 & 255) +'.' + (ipInt & 255) );
  }
  function ip2int(ip) {
    ip=ip.trim();
    return ip.split('.').reduce(function(ipInt, octet) { return (ipInt<<8) + parseInt(octet, 10)}, 0) >>> 0;
  }
  function SetHautBas(){
      let S="<div class='onglets'><div class='Bonglet Baccueil'><a href='/'>Accueil</a></div><div class='Bonglet Bbrut'><a href='/Brute'>Donn&eacute;es brutes</a></div><div class='Bonglet Bparametres'><a href='/Para'>Param&egrave;tres</a></div><div class='Bonglet Bactions'><a href='/Actions'>Actions</a></div></div>";
      S +="<div id='onglets2'><div class='Bonglet2 Bgeneraux'><a href='/Para'>Généraux</a></div><div class='Bonglet2 Bheure' ><a href='/Heure'>Heure</a></div><div class='Bonglet2 Bexport'><a href='/Export'>Import / Export</a></div><div class='Bonglet2 Bota'><a href='/OTA'>Mise à jour par OTA</a></div>";
      S +="<div id='Bwifi' class='Bonglet2 Bwifi'><a href='/Wifi'>WIFI</a></div><div class='Bonglet2 Bcouleurs'><a href='/Couleurs'>Couleurs</a></div></div>";
      S +="<h2 id='nom_R'>Routeur Solaire - RMS</h2>";
      GH("lesOnglets",S);
      GH("pied","<div>Routeur Version : <span id='version'></span></div><div><a href='https:F1ATB.fr/fr' target='_blank' >F1ATB.fr</a></div>");
  }
// ================================
// Conversion décimale -> HH:MM
// ================================
  function Hdeci2Hmn(H){
    let HI=parseInt(H);
    return Math.floor(HI / 100) + ":" + ("0" + Math.floor(0.6 * (HI +0.4 - 100 * Math.floor(HI / 100)))).substr(-2, 2);
  }
  function Hmn2Hdeci(H){
    let separ=":";
    if (H.indexOf(".")>0) separ=".";
    if (H.indexOf("h")>0) separ="h";
    let val=H.split(separ);
    let h = Math.floor(100*parseInt(val[0]) + 0.4 + 100*parseInt(val[1])/60);
    h=Math.max(0,h);h=Math.min(2400,h);
    return h;  
  }
  function Reset(){
      GID("attente").style="visibility: visible;";
      const xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
            GID('BoutonsBas').innerHTML="⏳ " + this.responseText +" ⏳";
            setInterval(location.reload(),2000);
          }         
        };
        xhttp.open('GET', '/restart', true);
        xhttp.send();
  }

  // ==========================
// LaVal - formatage aligné
// ==========================
function LaVal(d) {
  const n = parseInt(d, 10) || 0;
  const s = ('           ' + n.toString()).slice(-12); // garantir longueur suffisante
  // retourne format xx xxx xxx  (3 groupes)
  return `${s.substr(-9, 3)} ${s.substr(-6, 3)} ${s.substr(-3, 3)}`;
}

)====";

