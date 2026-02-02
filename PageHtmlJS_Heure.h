//********************************************************
// Page HTML et Javascript Mise à l'heure horloge interne *
//********************************************************
const char *HeureHtml = R"====(
  <!doctype html>
  <html><head><meta charset="UTF-8">
  <link rel="stylesheet" href="/commun.css">
  <style>
    .Zone{width:100%;border: 1px solid grey;border-radius:10px;margin:auto;background-color:rgba(30,30,30,0.3);} 
    .form {margin:auto;padding:10px;display: table;text-align:left;width:100%;}
    .ligne {display: table-row;padding:10px;}
    label{display: table-cell;margin: 5px;text-align:right;font-size:20px;height:25px;}
    input,select{display: table-cell;margin: 5px;text-align:left;font-size:20px;height:25px;}
    #onglets2{display:block;}
    .Bparametres{border:inset 10px azure;}
    .Bheure{border:inset 4px azure;}
    .bouton{width:auto;}
    .boldT{text-align:left;font-weight:bold;padding:10px;} 
    .spaceAR{display: flex; justify-content: space-around;}
    .space{display: flex; justify-content: center;}
    .Freset{margin: 5px;text-align:left;font-size:10px;height:25px;padding-top:15px;}
  </style>
  <title>Set hours F1ATB</title>
  </head>
  <body onload="Init();">
    <div id="lesOnglets"></div>
    <h2>Mise à l'heure horloge de l'ESP32</h2>
    <h4>Heure actuelle : <span id='date'>--------</span></h4>
    <div class="Zone">
        <div class="boldT">Horloge du Routeur</div>
        <div class="spaceAR">        
            <div><label for='Hor0'  id='Hor0L'>Internet</label><input type='radio' name='Horlo' id='Hor0' value="0" checked  onclick="checkDisabled();"  ></div>
            <div><label for='Hor1' >Linky</label><input type='radio' name='Horlo' id='Hor1' value="1" onclick="checkDisabled();"  ></div>
            <div><label for='Hor2' >Interne</label><input type='radio' name='Horlo' id='Hor2' value="2" onclick="checkDisabled();"  ></div>
            <div><label for='Hor3' >IT 10ms/100Hz (Triac)</label><input type='radio' name='Horlo' id='Hor3' value="3" onclick="checkDisabled();"  > </div>  
            <div><label for='Hor4' >IT 20ms/50Hz</label><input type='radio' name='Horlo' id='Hor4' value="4" onclick="checkDisabled();"  > </div>  
            <div><label for='Hor5' >ESP Externe</label><input type='radio' name='Horlo' id='Hor5' value="5" onclick="checkDisabled();"  > </div>           
        </div>
    </div>
    <br>
    <div class="Zone" id="ZoneNew_H">
        <div class="form"  >
          <div class='ligne' >
            <label for='New_J'  >Année/Mois/Jour <small>JJ/MM/AAAA</small></label><input type='text'  id='New_J' value=""    placeholder="25/12/2025">
            <label for='New_H'  >Heure  hh:mn</label><input type='text'  id='New_H' value=""  placeholder="15:36" >
          </div>
        </div>
    </div>
    <br>
    <div class="Zone" id="ZoneFuseau">     
          <div class='spaceAR' >
            <div>
              <label for='Fuseau'>Fuseau horaire : <br><small>&nbsp;</small></label>
              <select id='Fuseau' >
                <option value='0'selected>Europe centrale</option>
                <option value='1'>Guadeloupe / Martinique</option>
                <option value='2'>Guyane</option>
                <option value='3'>Réunion</option>
                <option value='4'>Mayotte</option>
                <option value='5'>Nouvelle Calédonie</option>
                <option value='6'>Wallis et Futuna</option>
              </select>
              
            </div>
            <div>
              <label for='ntpServer'  >Serveur NTP (Option)<br><small>ex: mafreebox.free.fr </small></label><input type='text'  id='ntpServer' value="" title='Laissez vide par defaut'>
            </div>
          </div>
          <div class='Freset'>* Nécessite une sauvegarde puis un Reset pour prise en compte</div>
    </div>
    <div id="BoutonsBas">        
        <input  class="bouton" type="button" onclick="SendHour();" value="Sauvegarder" >
        <div class="lds-dual-ring" id="attente"></div>
        <input  class='bouton' type='button' onclick='Reset();' value='ESP32 Restart' >
    </div>
    <script>    
        var BordsInverse=[".Bparametres",".Bheure"];
        function Init(){
          SetHautBas();
          LoadParaFixe();
          LoadData();
         
        }

        function SetParaFixe(){
          GID("Hor"+ F.Horloge).checked = true;
          GID("Fuseau").value = F.idxFuseau;
          GID("ntpServer").value = F.ntpServer;
          checkDisabled();
           Set_Couleurs();
        }
        function LoadData() {
          let xhttp = new XMLHttpRequest();
          xhttp.onreadystatechange = function() { 
            if (this.readyState == 4 ) {
              if (this.status == 200) {
                let DuRMS=this.responseText;
                let groupes=DuRMS.split(GS);
                let G0=groupes[0].split(RS);
                GID('date').innerHTML = G0[1];  
              }
              setTimeout('LoadData();',2000);
            }
          };
          xhttp.open('GET', '/ajax_data', true);
          xhttp.send();
        }
        function SendHour(){
          
          GID("attente").style="visibility: visible;";
          var xhttp = new XMLHttpRequest();
          xhttp.onreadystatechange = function() { 
            if (this.readyState == 4 && this.status == 200) {
              GID("attente").style="visibility: hidden;";
              location.reload();
            }         
          };
          var New_H= GID("New_H").value.trim() ;
          var New_J= GID("New_J").value.trim() ;
          F.idxFuseau= GID("Fuseau").value ;
          F.Horloge = document.querySelector('input[name="Horlo"]:checked').value;
          F.ntpServer=GID("ntpServer").value.trim();      
          xhttp.open('GET', 'HourUpdate?New_H='+encodeURIComponent(New_H)+'&New_J='+New_J +'&Horloge='+F.Horloge +'&idxFuseau='+F.idxFuseau +'&ntpServer='+F.ntpServer, true);
          xhttp.send(); 
        }
         function checkDisabled(){
          if (F.ModeReseau>0 && F.Horloge==0) {GID("Hor2").checked = true;F.Horloge=2;}
          F.Horloge = document.querySelector('input[name="Horlo"]:checked').value;
          GID("ZoneNew_H").style.display= (F.Horloge<2 || F.Horloge>4) ? "none": "block";
          GID("ZoneFuseau").style.display= (F.Horloge==0) ? "block": "none";
        }
    </script>
    <br>
    <div id="pied"></div>
    <br>
    <script src="/ParaCommunJS"></script>
    <script src="/CommunCouleurJS"></script>
  </body></html>
 
 )====";