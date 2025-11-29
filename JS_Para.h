//***************************************************
// Page Javascript  de gestion des Paramètres
//***************************************************
const char * ParaJS = R"====(
var refTempIP = [];
var BordsInverse = [".Bparametres", ".Bgeneraux"];
function Init() {
  var S = "";
  for (var i = 0; i < 4; i++) {

    S += "<div class='form cadre'>";
      S +='<div class="ligne" >';
        S +='<label for="srcTemp' + i +'">Canal :' + i + '</label>';
        S +='<select id="srcTemp' + i +'" onclick="checkDisabled();" ><option value="tempNo" selected>Pas de mesure</option><option value="tempInt">Capteur DS18B20 Interne</option>';
        S +='<option value="tempExt">Capteur DS18B20 ESP Externe</option><option value="tempMqtt">MQTT</option></select>';  
      S +='</div>';

    S += "</div>";
    S += "<div class='form ligneTemperature' id='ligneTemperature" + i + "' >";
      S += "<div class='ligne' >";
        S += "<label for='nomTemperature" + i + "' >Nom Température : </label>";
        S += "<input type='text' name='nomTemperature" + i + "' id='nomTemperature" + i + "' >";
      S += "</div>";
      S += "<div class='ligne' id='ligneoffsetTemp" + i + "'>";
        S += "<label for='offsetTemp" + i + "' >Offset mesure température (°C) :</label>";
        S += "<input type='number' name='offsetTemp" + i + "' id='offsetTemp" + i + "' >";
      S += "</div>";
      S += "<div class='ligne' id='ligneTopicT" + i + "'>";
        S += "<label for='TopicT" + i + "' >MQTT Topic Température :</label>";
        S += "<input type='text' name='TopicT" + i + "' id='TopicT" + i + "' >";
      S += "</div>";
      S += "<div class='ligne' id='ligneIPtemp" + i + "'>";
        S += "<label for='refTempIP" + i + "' >Adresse IP ESP-RMS externe :</label>";
        S += "<select  id='refTempIP" + i + "' ></select>";
      S += "</div>";
      S += "<div class='ligne' id='canalEXTtemp" + i + "'>";
        S += "<label for='canalEXT" + i + "' >Canal Température ESP-RMS externe :</label>";
        S += "<select id='canalEXT" + i + "'><option value=0>Canal 0</option><option value=1>Canal 1</option><option value=2>Canal 2</option><option value=3>Canal 3</option></select>";
      S += "</div>";
    S += "</div>";

  }
  GH("Sources_Temp", S);
  S = "<div class='ligne'><div class='nomR'><strong>Nom</strong></div><div class='nomR'><strong>Adresse IP</strong></div></div>";
  for (var i = 0; i < 8; i++) {
    S += "<div class='ligne' id='Routeur_" + i + "' style='display:none;' >";
    var bg = "";
    if (i == 0) bg = "style='background-color:#bbb;'";
    S += "<div id='RMS_Nom" + i + "'  class='nomR'></div><input type='text' id='RMS_IP" + i + "' " + bg + ">";
    S += "</div>";
  }
  GH("Routeurs", S);

  SetHautBas();
  LoadParametres();
  LoadParaRouteur();
  LoadCouleurs();
}
function LoadParametres() {
  //Cookie clé d'accès
  var name = "CleAcces=";
  var ca = document.cookie.split(';');
  for (var i = 0; i < ca.length; i++) {
    var c =  decodeURIComponent(ca[i]);
    while (c.charAt(0) == ' ') {
      c = c.substring(1);
    }
    if (c.indexOf(name) == 0) GID("CleAcces").value = c.substring(name.length, c.length);
  }


  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var BlocsParas = this.responseText;
      var LesParas = BlocsParas.split(GS);
      var Para = LesParas[0].split(RS);
      GID("dhcp").checked = Para[0] == 1 ? true : false;
      GID("adrIP").value = int2ip(Para[1]);
      GID("gateway").value = int2ip(Para[2]);
      GID("masque").value = int2ip(Para[3]);
      GID("dns").value = int2ip(Para[4]);
      GID("ModeP" + Para[5]).checked = true;
      ModePara = Para[5];
      GID("ModeW").value = Para[6];
      GID("Horlo").value = Para[7];
      GID("sources").value = Para[8];
      GID("RMSextIP").value = int2ip(Para[9]);
      GID("EnphaseUser").value = Para[10];
      GID("EnphasePwd").value = Para[11];
      GID("EnphaseSerial").value = Para[12];
      GID("TopicP").value = Para[13];
      GID("MQTTRepete").value = Para[14];
      GID("MQTTIP").value = int2ip(Para[15]);
      GID("MQTTPort").value = Para[16];
      GID("MQTTUser").value = Para[17];
      GID("MQTTpwd").value = Para[18];
      GID("MQTTPrefix").value = Para[19];
      GID("MQTTPrefixEtat").value = Para[20];
      GID("MQTTdeviceName").value = Para[21];
      GID("subMQTT").checked = Para[22] == 1 ? true : false;
      GID("nomRouteur").value = Para[23];
      GID("nomSondeFixe").value = Para[24];
      GID("nomSondeMobile").value = Para[25];
      GID("CalibU").value = Para[26];
      GID("CalibI").value = Para[27];
      GID("TempoRTEon").checked = Para[28] == 1 ? true : false;
      GID("WifiSleep").checked = Para[29] == 1 ? true : false;
      GID("ComSurv").value = Para[30];
      GID("Serie").value = Para[31];
      GID("pTriac").value = Para[32];
      GID("ESP").value = Para[33];
      GID("pLED").value = Para[34];
      GID("Rot").value = Para[35];
      GID("DurEcran").value = Para[36];
      GID("pUxI").value = Para[37];
      GID("PTemp").value = + Para[38];
      GID("EstimCACSI").checked = (Para[39] == 1) ? true : false;
      for (var c = 0; c < 4; c++) {
        var ParaT = LesParas[c + 1].split(RS);
        GID("nomTemperature" + c).value = ParaT[0];
        GID("srcTemp" + c).value = ParaT[1];
        GID("TopicT" + c).value = ParaT[2];
        refTempIP[c] = ParaT[3];
        GID("canalEXT" + c).value = ParaT[4];
        GID("offsetTemp" + c).value = ParaT[5] / 100;
      }

      checkDisabled();
    }
  };
  xhttp.open('GET', '/ParaAjax', true);
  xhttp.send();
}
function SendValues() {
  GID("attente").style = "visibility: visible;";
  var dhcp = GID("dhcp").checked ? 1 : 0;
  var TempoRTEon = GID("TempoRTEon").checked ? 1 : 0;
  var Source_new = GID("sources").value;
  var subMQTT = GID("subMQTT").checked ? 1 : 0;
  var WifiSleep = GID("WifiSleep").checked ? 1 : 0;
  ModePara = document.querySelector('input[name="ModeP"]:checked').value;
  var EstimCACSI = GID("EstimCACSI").checked ? 1 : 0;
  if (Source_new != "Linky") EstimCACSI = 0;
  if (ModePara == 0) { //Non Expert
    subMQTT = 0; WifiSleep = 1;
  }
  var S = dhcp + RS + ip2int(GID("adrIP").value) + RS + ip2int(GID("gateway").value);
  S += RS + ip2int(GID("masque").value) + RS + ip2int(GID("dns").value) + RS + ModePara + RS + GID("ModeW").value + RS + GID("Horlo").value;
  S += RS + Source_new + RS + ip2int(GID("RMSextIP").value) + RS + GID("EnphaseUser").value.trim() + RS + GID("EnphasePwd").value.trim() + RS + GID("EnphaseSerial").value.trim() + RS + GID("TopicP").value.trim();
  S += RS + GID("MQTTRepete").value + RS + ip2int(GID("MQTTIP").value) + RS + GID("MQTTPort").value + RS + GID("MQTTUser").value.trim() + RS + GID("MQTTpwd").value.trim();
  S += RS + GID("MQTTPrefix").value.trim() + RS + GID("MQTTPrefixEtat").value.trim() + RS + GID("MQTTdeviceName").value.trim() + RS + subMQTT;
  S += RS + GID("nomRouteur").value.trim() + RS + GID("nomSondeFixe").value.trim() + RS + GID("nomSondeMobile").value.trim();
  S += RS + GID("CalibU").value + RS + GID("CalibI").value + RS + TempoRTEon + RS + WifiSleep + RS + GID("ComSurv").value + RS + GID("Serie").value + RS + GID("pTriac").value;
  S += RS + GID("ESP").value + RS + GID("pLED").value + RS + GID("Rot").value + RS + GID("DurEcran").value + RS + GID("pUxI").value + RS + GID("PTemp").value + RS + EstimCACSI;
  for (var c = 0; c < 4; c++) {
    var Qtemp = GID("srcTemp"+c).value;
    var Source_Temp = (ModePara == 0) ? "tempNo" : Qtemp;
    S += RS + GID("nomTemperature" + c).value.trim() + RS + Source_Temp + RS + GID("TopicT" + c).value.trim() + RS + GID("refTempIP" + c).value + RS + GID("canalEXT" + c).value + RS + GID("offsetTemp" + c).value * 100;
  }
  for (var i = 0; i < 8; i++) {
    var Vip = (ModePara == 0) ? 0 : ip2int(GID("RMS_IP" + i).value.trim());
    S += RS + Vip;
  }
  S = "?lesparas=" + encodeURIComponent(S);
  let cook = GID("CleAcces").value.trim();
  document.cookie = "CleAcces=" + encodeURIComponent(cook);
  if ((GID("dhcp").checked || checkIP("adrIP") && checkIP("gateway")) && (!GID("MQTTRepete").checked || checkIP("MQTTIP"))) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
      if (this.readyState == 4 && this.status == 200) {
        var retour = this.responseText;
        location.reload();
      }
    };
    xhttp.open('GET', 'ParaUpdate' + S, true);
    xhttp.send();
  }
}

function SendRAZ() {
  if (confirm("Confirmez la remise à zéro des historiques")) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
      if (this.readyState == 4 && this.status == 200) {
        var retour = this.responseText;
      }
    };
    xhttp.open('GET', '/ajaxRAZhisto', true);
    xhttp.send();
  }
}
function checkDisabled() {
  GID("RMS_IP0").style.backgroundColor = "#888";
  GID("RMS_IP0").readOnly = true;
  ModeReseau = GID("ModeW").value;
  ModePara = document.querySelector('input[name="ModeP"]:checked').value;
  Horloge = GID("Horlo").value;
  var pESP = GID("ESP").value;
  GID("Bheure").style.display = (Horloge > 1 && Horloge < 5) ? "inline-block" : "none";
  GID("Bwifi").style.display = (ESP32_Type < 10) ? "inline-block" : "none";
  GID("infoIP").style.display = (GID("dhcp").checked || ModeReseau == 2) ? "none" : "table";
  GID("dhcp").style.visibility = (ModeReseau == 2) ? "hidden" : "visible";
  GID("ipreset").style.display = (ModeReseau == 2) ? "none" : "inherit";
  GID("rotation").style.display = (pESP == 4) ? "table-row" : "none";
  GID("dureeOn").style.display = (pESP == 4) ? "table-row" : "none";
  GID("l_wifi_0").style.display = (ModeReseau == 2) ? "none" : "table-row";
  GID("l_wifi_1").style.display = (ModeReseau == 2 || ModePara == 0) ? "none" : "table-row";
  GID("l_wifi_2").style.display = (ModeReseau == 2 || ModePara == 0 || ESP32_Type >= 10) ? "none" : "table-row";

  GID("listerouteurs").style.display = (ModeReseau == 2 || ModePara == 0) ? "none" : "block";
  for (var i = 5; i < 11; i++) {
    GID("sources").options[i].disabled = (ModeReseau == 2) ? true : false;
  }
  GID("sources").options[11].disabled = (ModePara == 0 || ModeReseau == 2) ? true : false;
  for (var i = 0; i < 4; i++) {
    var QS='#srcTemp'+i + ' option[value="tempExt"]';
    document.querySelector(QS).disabled = (ModeReseau == 2) ? true : false;
    QS='#srcTemp'+i + ' option[value="tempMqtt"]';
    document.querySelector(QS).disabled = (ModeReseau == 2) ? true : false;
  }
  GID("donEnv").style.display = (ModeReseau == 2) ? "none" : "inline-block;";
  if (ModeReseau == 2) {
    GH("localIP", " : 192.168.4.1");
  } else {
    GH("localIP", "auto (DHCP) : " + LocalIP);
  }
  GID("Horlo").options[0].disabled = (ModeReseau > 0) ? true : false;
  GID("Horlo").options[5].disabled = (Source == 'Ext' && GID("RMSextIP").value != "") ? false : true;
  if (ModeReseau > 0 && Horloge == 0) { GID("Horlo").value = 2; Horloge = 2; }
  GID("Zmqtt").style.display = ((GID("MQTTRepete").value != 0 || GID("sources").value == 11 || GID("subMQTT").checked) && ModePara > 0) ? "block" : "none";
  GID("LesSourcesTemp").style.display = (ModePara == 0) ? "none" : "block";
  GID("pTemp").style.display = (ModePara == 0) ? "none" : "table-row";
  GID("SurvCom").style.display = (ModePara == 0) ? "none" : "block";
  for (var c = 0; c < 4; c++) {
    GID('ligneTemperature' + c).style.display = (GID("srcTemp" + c).value=="tempNo") ? "none" : "table";
    GID('ligneTopicT' + c).style.display = (GID("srcTemp" + c).value=="tempMqtt") ? "table-row" : "none";
    GID('ligneIPtemp' + c).style.display = (GID("srcTemp" + c).value=="tempExt" ) ? "table-row" : "none";
    GID('canalEXTtemp' + c).style.display = (GID("srcTemp" + c).value=="tempExt" ) ? "table-row" : "none";
    GID('ligneoffsetTemp' + c).style.display = (GID("srcTemp" + c).value=="tempInt" ) ? "table-row" : "none";
    if (GID("srcTemp" + c).value=="tempMqtt" ) GID("Zmqtt").style.display = "table";
  }
  GID('ligneTopicP').style.display = (GID("sources").value == 11) ? "table-row" : "none";
  Source = GID("sources").value;
  if (Source != 'Ext') Source_data = Source;
  AdaptationSource();
}
function checkIP(id) {
  var S = GID(id).value;
  var Table = S.split(".");
  var valide = true;
  if (Table.length != 4) {
    valide = false;
  } else {
    for (var i = 0; i < Table.length; i++) {
      if (Table[i] > 255 || Table[i] < 0) valide = false;
    }
  }
  if (valide) {
    GID(id).style.color = "black";
  } else {
    GID(id).style.color = "red";
  }
  return valide;
}



function AdaptationSource() {
  GID('ligneFixe').style.display = (Source_data == 'UxIx2' || ((Source_data == 'ShellyEm' || Source_data == 'ShellyPro') && GID("EnphaseSerial").value != 3)) ? "table-row" : "none";
  GID('Zcalib').style.display = (Source_data == 'UxI' && Source == 'UxI') ? "table" : "none";
  GID('Analog').style.display = (Source_data == 'UxI' && Source == 'UxI') ? "table-row" : "none";
  GID('CACSI').style.display = (Source == 'Linky') ? "table" : "none";
  var txtExt = "ESP-RMS";
  if (Source == 'Enphase') txtExt = "Enphase-Envoy";
  if (Source == 'SmartG') txtExt = "SmartGateways";
  if (Source == 'HomeW') txtExt = "HomeWizard";
  var lab_enphaseShelly = "Numéro série passerelle IQ Enphase : <span class='fsize10'><br>Pour firmvare Envoy-S V7 seulement</span>";
  if (Source == 'ShellyEm' || Source == 'ShellyPro') {
    txtExt = "Shelly (Pro) Em ";
    lab_enphaseShelly = "<div class='shem'><strong>Shelly (Pro) Em</strong><br>";
    lab_enphaseShelly += "Monophasé : Courant maison sur voie 0,1 ou 2<br>Triphasé : mettre 3";
    lab_enphaseShelly += "</div><div class='shem'><Strong>Shelly Em Gen3</strong><br>";
    lab_enphaseShelly += "Courant maison sur voie 0 = 30, voie 1 = 31</div>";
  }
  GID('labExtIp').innerHTML = txtExt;
  GID('label_enphase_shelly').innerHTML = lab_enphaseShelly;
  GID('ligneExt').style.display = (Source == 'Ext' || Source == 'Enphase' || Source == 'SmartG' || Source == 'HomeW' || Source == 'ShellyEm' || Source == 'ShellyPro') ? "table-row" : "none";
  GID('ligneEnphaseUser').style.display = (Source == 'Enphase') ? "table-row" : "none";
  GID('ligneEnphasePwd').style.display = (Source == 'Enphase') ? "table-row" : "none";
  GID('ligneEnphaseSerial').style.display = (Source == 'Enphase' || Source == 'ShellyEm' || Source == 'ShellyPro') ? "table-row" : "none"; //Numéro de serie ou voie
}
function FinParaRouteur() {
  var Soptions = "";
  for (var c = 0; c < nb_ESP; c++) {
    GID("RMS_IP" + c).value = IP_RMS[c];
    GH("RMS_Nom" + c, nomRMS[c]);
    GID("Routeur_" + c).style = "display: table-row";
    if (c > 0) {
      Soptions += "<option value=" + c + " >" + IP_RMS[c] + " " + nomRMS[c] + "</option>";
    }
    var d = c + 1;
    if (c < 7) {
      GH("RMS_Nom" + d, "<small>Ajoutez une adresse IP de routeur ----></small>");
      GID("Routeur_" + d).style = "display: table-row";
    }
  }

  for (var i = 0; i < 4; i++) {
    GH("refTempIP" + i, Soptions);
    GID("refTempIP" + i).value = refTempIP[i];
  }
  setTimeout('checkDisabled();', 500);

}
)====";

