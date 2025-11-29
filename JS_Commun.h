//Paramètres du routeur et fonctions générales pour toutes les pages.
const char *ParaRouteurJS = R"====(
  var Source="";
  var Source_data="";
  var RMSextIP="";
  var ES=String.fromCharCode(27); //Escape Separator
  var FS=String.fromCharCode(28); //File Separator
  var GS=String.fromCharCode(29); //Group Separator
  var RS=String.fromCharCode(30); //Record Separator
  var US=String.fromCharCode(31); //Unit Separator
  var nomSondeFixe="Sonde Fixe";
  var nomSondeMobile="Sonde Mobile";
  var nb_ESP = 0;
  var nomRMS=[];
  var IP_RMS=[];
  var nomTemperature=[];
  var nomActions=[];
  var ModeReseau=0;
  var ModePara=0;
  var Horloge=0;
  var LocalIP ="";
  var ESP32_Type =0;
  
  function LoadParaRouteur() {
    var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
             var LesParas=this.responseText;
             var Para=LesParas.split(GS);
             Source=Para[0];
             Source_data=Para[1];
             LocalIP=Para[2];
             RMSextIP= Para[7]; 
             AdaptationSource();  
             GH("nom_R",Para[3]);
             GH("version",Para[4]);
             GH("nomSondeFixe",Para[5]);
             GH("nomSondeMobile",Para[6]); 
             nomSondeFixe=Para[5];
             nomSondeMobile=Para[6];
             ModeReseau=Para[8];
             ModePara=Para[9];
             Horloge=Para[10];
             ESP32_Type=Para[11];
             var IpNoms=Para[12].split(RS);
             nb_ESP=IpNoms.length - 1;
             for (var c=0;c<nb_ESP;c++){
              var ip_nom=IpNoms[c].split(US);
              IP_RMS[c]=int2ip(ip_nom[0]); 
              nomRMS[c]=ip_nom[1];
              if (c==0) {
                var LesTemp=ip_nom[2].split(FS); //Noms temperature ESP local
                for (var i=0;i<LesTemp.length -1;i++){
                  var LaTemp=LesTemp[i].split(ES);
                  nomTemperature[LaTemp[0]] = LaTemp[1];
                }
              }
              var lesNomsActions=ip_nom[3].split(FS);
              nomActions[c]=[]; 
              for (var i=0;i<lesNomsActions.length -1;i++){
                  var Nact=lesNomsActions[i].split(ES);
                  nomActions[c][i]=Nact;
              }
             }
             IP_RMS[0] = Para[2]; 
             document.title=nomRMS[0] +" - " + document.title;
             nomRMS[0] = nomRMS[0] +" (local)";
             FinParaRouteur();
          
          }         
        };
        xhttp.open('GET', '/ParaRouteurAjax', true);
        xhttp.send();
  }
  function GID(id) { return document.getElementById(id); }
  function GH(id, T) {
    if ( GID(id)){
     GID(id).innerHTML = T; }
    }
  function GV(id, T) { GID(id).value = T; }
  
  function int2ip (V) {
    var ipInt=parseInt(V);
    return ( (ipInt>>>24) +'.' + (ipInt>>16 & 255) +'.' + (ipInt>>8 & 255) +'.' + (ipInt & 255) );
  }
  function ip2int(ip) {
    ip=ip.trim();
    return ip.split('.').reduce(function(ipInt, octet) { return (ipInt<<8) + parseInt(octet, 10)}, 0) >>> 0;
  }
  function SetHautBas(){
      var S="<div class='onglets'><div class='Bonglet Baccueil'><a href='/'>Accueil</a></div><div class='Bonglet Bbrut'><a href='/Brute'>Donn&eacute;es brutes</a></div><div class='Bonglet Bparametres'><a href='/Para'>Param&egrave;tres</a></div><div class='Bonglet Bactions'><a href='/Actions'>Actions</a></div></div>";
      S +="<div id='onglets2'><div class='Bonglet2 Bgeneraux'><a href='/Para'>Généraux</a></div><div class='Bonglet2 Bexport'><a href='/Export'>Import / Export</a></div><div class='Bonglet2 Bota'><a href='/OTA'>Mise à jour par OTA</a></div><div id='Bwifi' class='Bonglet2 Bwifi'><a href='/Wifi'>WIFI</a></div>";
      S +="<div class='Bonglet2 Bheure' id='Bheure'><a href='/Heure'>Heure</a></div><div class='Bonglet2 Bcouleurs'><a href='/Couleurs'>Couleurs</a></div></div>";
      S +="<h2 id='nom_R'>Routeur Solaire - RMS</h2>";
      GH("lesOnglets",S);
      GH("pied","<div>Routeur Version : <span id='version'></span></div><div><a href='https:F1ATB.fr/fr' >F1ATB.fr</a></div>");
  }
  function Hdeci2Hmn(H){
    var HI=parseInt(H);
    return Math.floor(HI / 100) + ":" + ("0" + Math.floor(0.6 * (HI +0.4 - 100 * Math.floor(HI / 100)))).substr(-2, 2);
  }
  function Hmn2Hdeci(H){
    var separ=":";
    if (H.indexOf(".")>0) separ=".";
    if (H.indexOf("h")>0) separ="h";
    var val=H.split(separ);
    var h = Math.floor(100*parseInt(val[0]) + 0.4 + 100*parseInt(val[1])/60);
    h=Math.max(0,h);h=Math.min(2400,h);
    return h;  
  }
  function Reset(){
      GID("attente").style="visibility: visible;";
      var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
            GID('BoutonsBas').innerHTML=this.responseText;
            GID("attente").style="visibility: hidden;";
            setTimeout(location.reload(),1000);
          }         
        };
        xhttp.open('GET', '/restart', true);
        xhttp.send();
  }

)====";

