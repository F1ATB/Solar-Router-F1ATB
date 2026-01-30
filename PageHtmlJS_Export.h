//********************************************************
// Page HTML et Javascript Import / Export configuration *
//********************************************************
const char *ExportHtml = R"====(
  <!doctype html>
  <html><head><meta charset="UTF-8">
  <link rel="stylesheet" href="/commun.css">
  <style>
    .Zone{width:100%;border:1px solid grey;border-radius:10px;margin-top:10px;background-color:rgba(30,30,30,0.3);} 
    .boldT{text-align:left;font-weight:bold;padding:10px;}
    .form {margin:auto;padding:10px;display: table;text-align:left;width:100%;}
    .ligne {display: table-row;padding:10px;}
    label{display:table-cell;margin:5px;text-align:left;font-size:20px;height:25px;width:60%;}
    input,.cell{display: table-cell;margin: 5px;text-align:left;font-size:20px;height:25px;}
    input[type=file]{display:block;height:32px;border:none;}
    .liste{display:flex;justify-content:center;text-align:left;} 
    #onglets2{display:block;}
    .Bparametres{border:inset 10px azure;}
    .Bexport{border:inset 4px azure;}
    #memoire{display:flex;justify-content:space-around;font-size:20px;}
  </style>
  <title>Import/Export</title>
  </head>
  <body onload="Init();">
    <div id="lesOnglets"></div>
    <h2 >Import / Export des paramètres et données</h2>
    <div class="Zone">
        <div class="boldT">Liste des fichiers à l'export<br><span class='fsize12'>(Permet la sauvegarde des paramètres et données sur votre PC)</span></div>
        <div class="form" id="liste_file" ></div>
        <div id="memoire"></div>
    </div>
   
    <div class="Zone">
        <div class="boldT">Import des paramètres (.json) ou d'un fichier de données mensuelles (.csv)</div>
        <form method="POST" onsubmit="submit_para(event);" action="#" enctype="multipart/form-data" id="upload_form">
          <div class="form">
            <div class="ligne">
              <div class="cell"><input type="file" name="fichier_para_" id="fichier_para_"  class="bouton" accept=".json,.csv"></div>
              <input type="submit" value="Mettre à jour"  class="bouton">
            </div>
            <div class="lds-dual-ring" id="attente"></div>
          </div>
        </form>
        <span class="fsize12">Après un Import de paramètres, faites un Reset pour redémarrer avec les nouveaux.</span>
    </div>

    
    <input  class="bouton" type="button" onclick="Reset();" value="ESP32 Reset" >
    <script>
        var BordsInverse=[".Bparametres",".Bexport"]; 
        function Init(){
          SetHautBas();
          LoadParaFixe();
          
        }
        
        function submit_para(event){
          GID("attente").style="visibility: visible;";
          event.preventDefault(); //Pour Firefox
          let xhttp = new XMLHttpRequest();
          xhttp.onreadystatechange = function() { 
            if (this.readyState == 4 && this.status == 200) {
              let retour=this.responseText;
              location.reload();
              GID("attente").style="visibility: hidden;";
            }         
          };
          const fileInput = GID("fichier_para_");
          let file = fileInput.files[0];
          let data = new FormData();
          data.append("file", file);
          xhttp.open('POST', '/import', true);
          xhttp.send(data);
        }
        function LoadListeFile(){
          var xhttp = new XMLHttpRequest();
          xhttp.onreadystatechange = function() { 
            if (this.readyState == 4 && this.status == 200) {
              let data = this.responseText.split(FS);
              let files = data[0].split(GS);
              let S="";
              let MyIP=int2ip(V.IP_Fixe);
              for (var i=0;i<files.length-1;i++){
                let Fi=files[i].split(RS);
                let F_Long=Fi[0];
                if (F_Long=="parametres.json") F_Long="parametres_V" +F.VersionStocke +"_IP" + MyIP.substring(MyIP.lastIndexOf('.') + 1) +".json"; //On rajoute version et IP au nom
                S +="<div class='ligne'><div class='cell'>" +F_Long+" </div><div class='cell'>" +Fi[1]+" octets </div>";
                S +="<div class='cell'><a href='/export_file?Fichier=" + Fi[0] + "&download="+F_Long+"' ><button class='bouton'>Télécharger</button></a></div>";
                S +="<div class='cell'><button class='bouton' onclick='valideEfface(\""+Fi[0]+"\");'>Effacer</button></div></div>";
              }
              GH("liste_file", S);
              S='<div>Espace fichiers total : ' + data[1] + ' octets</div><div>Espace fichiers utilisé : ' + data[2] + ' octets = ' +Math.floor(data[2]*100/data[1]) + ' %</div>';
              GH("memoire", S);
            }         
          };
          xhttp.open('GET', 'ListeFile', true);
          xhttp.send();
        }
        function valideEfface(F){
          var Conf="Confirmez l'effacement";
          if (confirm(Conf)==true){
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() { 
              if (this.readyState == 4 && this.status == 200) {
                location.reload();
              }         
            };
            F ="/export_file?Fichier=" + F + "&Delete=OK";
            xhttp.open('GET', F, true);
            xhttp.send();
          }
        }
        function SetParaFixe(){
          LoadParaVar(); 
          GID("Bwifi").style.display= (F.ESP32_Type<10) ? "inline-block": "none";
          Set_Couleurs();
        }
        function SetParaVar(){
          LoadListeFile();
        }
        function AdaptationSource(){}
        
    </script>
    <br>
    <div id="pied"></div>
    <br>
    <script src="/ParaCommunJS"></script>
    <script src="/CommunCouleurJS"></script>
</body></html>
 
 )====";