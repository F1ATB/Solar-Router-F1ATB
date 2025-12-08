//*********************************************
// Page principale HTML
//*********************************************
const char *MainHtml = R"====(
<!doctype html>
<html>

<head>
  <meta charset="UTF-8">
  <link rel="stylesheet" href="/commun.css">
  <title>RMS F1ATB</title>
  <style>
    .grid-container2,.grid-container2M {
      display: grid;
      grid-template-columns: auto auto auto auto auto auto;
      grid-gap: 2px;
      background-color: rgba(30, 30, 30, 0.3);
      padding: 10px;
      border: 2px inset grey;
      border-radius: 10px;
      text-align: right;
      color:black;
    }
    .grid-container1 {
      display: grid;
      grid-template-columns: auto auto auto auto;
      grid-gap: 2px;
      background-color: rgba(30, 30, 30, 0.3);
      padding: 10px;
      border: 2px inset grey;
      border-radius: 10px;
      text-align: right;
      color:black;
    }

    .grid-container1>div,.grid-container2>div,.grid-container2M>div {     
      padding: 10px;
    }

    .item1 {
      grid-column: 2 / span 2;
      background-color: rgba(62, 194, 247, 0.5);
      font-weight: bold;
    }

    .item2 {
      grid-column: 4 / span 2;
      background-color: rgba(62, 194, 247, 0.5);
      font-weight: bold;
    }

    .item3 {
      grid-column-start: 6;
      background-color: rgba(62, 194, 247, 0.5);
    }
    .item31 {
      grid-column-start: 4;
      background-color: rgba(62, 194, 247, 0.5);
    }

    .item4 {
      grid-column-start: 2;
      background-color: rgba(62, 194, 247, 0.5);
      font-weight: bold;
    }

    .item5,
    .item6,
    .item7,
    .item14 {
      background-color: rgba(62, 194, 247, 0.5);
      font-weight: bold;
    }

    .item8 {
      grid-column-start: 1;
      text-align: left;
    }
    .item_Act{
      grid-column: 1 / span 2;
      background-color: rgba(62, 194, 247, 0.5);
      font-weight: bold;
    }
    .item_H{
      background-color: rgba(62, 194, 247, 0.5);
      font-weight: bold;
    }
    .item_val{
      background-color: rgba(200,200,200, 0.5);  
    }
    .item_F{
      background-color: rgba(250,250,200, 0.5);  
    }
    .item_Force{
      grid-column: 4 / span 3;
      background-color: rgba(62, 194, 247, 0.5);
      font-weight: bold;
    }
    .item_temp_nom{
        grid-column: 1 / span 2;
        background-color: #8f8;
    }
    .item_temp_val{
        grid-column: 3 / span 4;
        background-color: #8f8;
    }

    .le {
      text-align: left;
    }

    .ce {
      text-align: center;
    }

    .Wh {
      background-color: #ff8;
    }

    .W {
      background-color: #f88;
    }

    .VA {
      background-color: #0ff;
    }

    .deg {
      background-color: #fdf;
    }

    

    .foot {
      font-size: 16px;
    }

   

    svg {
      border: 2px inset grey;
      border-radius: 10px;
    }

    #LED {
      position: absolute;
      top: 4px;
      left: 4px;
      width: 0px;
      height: 0px;
      border: 5px solid red;
      border-radius: 5px;
    }

    .Baccueil {
      border: inset 8px azure;
    }

    .jauge {
      background-color: #ff8;
      height: 28px;
      text-align: left;
      overflow: visible;
      position: absolute;
      top: 4px;
      left: 4px;
    }

    .jaugeBack {
      background-color: rgb(0, 195, 255);
      width: 208px;
      height: 36px;
      position: relative;
      padding: 4px;
    }

    .w100 {
      width: 100%;
      position: absolute;
      top: 4px;
      left: 4px;
    }

    #♣TabMesures,
    #SVG_PW48hT,
    #SVG_PW2sT,
    .SVG_Temp48h,
    #SVG_Ouvertures,
    #SVG_Ouvertures_2s {
      display: none;
    }
   
    #donneeDistante {
      font-size: 50%;
      color: white;
      text-align: center;
      margin-bottom: 10px;
      display: none;
    }

    #info {
      position: absolute;
      border-left: 1px solid black;
      display: none;
    }

    #info_txt {
      position: absolute;
      background-color: rgba(120, 120, 120, 0.7);
      padding: 4px;
      right: 10px;
      border: 1px solid black;
      text-align: right;
    }

    #couleurTarif_jour,
    #couleurTarif_J1 {
      font-size: 10px;
    }

    #etatActions {
      display: none;
    }

  

    .choixG {
      text-align: left;
      width: 100%;
      height: 0px;
      position: relative;
    }

    .choix {
      position: absolute;
      left: 120px;
      top: -80px;
      font-size: 16px;
      display: none;
    }

    iframe {
      width: 100%;
      min-height: 900px;
    }

    .autreRMS {
      width: 100%;
      height: 36px;
      background-color: grey;
      color: white;
      font-size: 30px;
      display: flex;
      justify-content: space-between;
      margin-top: 4px;
    }

    .autreRif {
      width: 100%;
    }
    
  </style>
</head>

<body onload="Init();">
  <div id="LED" title="Réception des données"></div>
  <div id="lesOnglets"></div>
  <div id="date">DATE</div>
  <div id="TabMesures"></div>
  
  <br>
  
  <div id="donneeDistante">Données distantes</div>
  <div id="etatActions" class="grid-container2"></div>
  <p id="SVG_PW2sM"></p>
  <div class="choixG">
    <div class="choix" id="SVG_PW2sM_L"><label>VA</label><input type="checkbox" id="SVG_PW2sM_C"></div>
  </div>
  <p id="SVG_PW2sT"></p>
  <div class="choixG">
    <div class="choix" id="SVG_PW2sT_L"><label>VA</label><input type="checkbox" id="SVG_PW2sT_C"></div>
  </div>
  <p id="SVG_PW48hM"></p>
  <p id="SVG_PW48hT"></p>
  <p class="SVG_Temp48h" id="SVG_Temp48h0"></p>
  <p class="SVG_Temp48h" id="SVG_Temp48h1"></p>
  <p class="SVG_Temp48h" id="SVG_Temp48h2"></p>
  <p class="SVG_Temp48h" id="SVG_Temp48h3"></p>
  <p id="SVG_Ouvertures_2s"></p>
  <p id="SVG_Ouvertures"></p>
  <p id="SVG_Wh1an"></p>
  <div id="info">
    <div id="info_txt"></div>
  </div>
  <br><br>
  <div class="foot">Donn&eacute;es RMS<div id="source"></div>
  </div>
  <div id="autresRMS"></div>
  <div id="pied"></div>
  <script src="/ParaRouteurJS"></script>
  <script src="MainJS1"></script>
  <script src="MainJS2"></script>
  <script src="MainJS3"></script>
  <script src="/CommunCouleurJS"></script>
  <br>
</body>

</html>
)====";