//************************************************
// Page HTML de gestion des Actions
//************************************************

const char *ActionsHtml = R"====(
<!doctype html>
<html>

<head>
    <meta charset="UTF-8">
    <script src="/PinsActionsJS"></script>
    <script src="/CommunCouleurJS"></script>
    <link rel="stylesheet" href="/commun.css">

    <style>
        .Bactions {
            border: inset 8px azure;
        }

        .cadre {
            width: 100%;
            max-width: 1200px;
            margin: auto;
        }

        .form {
            width: 100%;
            text-align: left;
        }


        .form2 {
            margin: auto;
            padding: 5px;
            display: table;
            text-align: left;
            width: 100%;
        }

        .titre {
            display: flex;
            justify-content: center;
            cursor: pointer;
            color: white;
            font-weight: bold;
            font-size: 110%;
        }

        .visu {
            position: absolute;
            top: 0px;
            right: 4px;
            cursor: pointer;
        }

        .planning,
        #CACSI,
        #Freq_PWM {
            width: 100%;
            position: relative;
            margin: 4px;
            margin-bottom: 10px;
            padding: 2px;
            border: 2px inset grey;
            background-color: rgba(30, 30, 30, 0.3);
            color: black;
            border-radius: 10px;
        }

        #CACSI,
        #Freq_PWM {
            background-color: #ffa;
        }

        #commun,
        #CACSI {
            display: none;
        }

        .periode {
            position: absolute;
            height: 100%;
            border: outset 4px;
            border-radius: 5px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            color: white;
            cursor: ew-resize;
        }

        .curseur {
            position: relative;
            width: 100%;
            height: 30px;
        }

        .infoAction {
            position: relative;
            display: flex;
            width: 100%;
            min-height: 40px;
            font-size: 20px;
        }

        .graphAction {
            position: relative;
            font-size: 20px;
            display: none;
            margin: 4px;
        }

        .graphSVG {
            width: 100%;
            height: 100%;
        }

        .GraphSVG {
            position: absolute;
            bottom: 8px;
            right: 70px;
            cursor: pointer;
            border: 2px outset grey;
            background-color: white;
            width: 25px;
            height: 30px;
            text-align: center;
        }

        .infoZone {
            position: relative;
            display: flex;
            justify-content: space-around;
            border: 1px solid grey;
            border-radius: 5px;
            align-items: center;
            text-align: center;
            background-color: #ddd;
            cursor: pointer;
        }

        .infoZone div {
            display: inline-block;
        }

        .infoZ {
            position: absolute;
            bottom: 10px;
            left: 10px;
            display: none;
            border: 8px inset #555;
            border-radius: 8px;
            background-color: #ddd;
            z-index: 1000;
            font-size: 16px;
        }

        .infoZ input {
            width: 70px;
        }

        .radioC {
            border: 3px outset grey;
            border-radius: 6px;
            display: inline-block;
            text-align: left;
            width: 220px;
            background-color: rgba(50, 50, 50, 0.3);
        }

        .radioC input {
            width: 20px;
            cursor: pointer;
        }

        .Hfin {
            position: absolute;
            bottom: 2px;
            right: 2px;
        }

        .zOff {
            background-color: #66f;
            margin: 4px;
            padding: 4px;
            border-radius: 8px;
        }

        .zOn {
            background-color: #f66;
            margin: 2px;
            padding: 4px;
            border-radius: 8px;
        }

        .zPw {
            background-color: #6f6;
            margin: 2px;
            padding: 4px;
            border-radius: 8px;
        }

        .zTriac {
            background-color: #cc4;
            margin: 2px;
            padding: 4px;
            border-radius: 8px;
        }

        .selectZ {
            background-color: #aaa;
            height: 20px;
            font-size: 16px;
            font-weight: bold;
            padding-left: 6px;
        }

        .closeZ {
            background-color: black;
            border: outset 2px grey;
            color: white;
            position: absolute;
            top: 0px;
            height: 20px;
            width: 20px;
            right: 0px;
            cursor: pointer;
        }

        .fcontainer {
            display: flex;
            justify-content: center;
            background-color: #eea;
            margin: 2px;
            border-top: 2px solid grey;
        }

        .fcontleft {
            display: flex;
            justify-content: space-around;
            flex-direction: column;
        }

        .tm {
            width: 60px;
            text-align: left;
        }

        .tbut {
            width: 40px;
            padding-left: 10px;
            padding-right: 10px;
            text-align: center;
            font-weight: bold;
            font-size: 24px;
            cursor: pointer;
            display: inline-flex;
        }

        .ligne {
            display: table-row;
            padding: 10px;
        }

        tr {
            margin: 2px;
            text-align: left;
            font-size: 20px;
        }

        h4 {
            padding: 2px;
            margin: 0px;
        }

        h5 {
            text-align: left;
            padding: 2px;
            margin: 0px;
        }

        label {
            text-align: right;
        }

        .source label {
            display: table-cell;
            margin: 5px;
            text-align: left;
            font-size: 20px;
            height: 26px;
            width: initial;
        }

        input {
            margin: 5px;
            text-align: left;
            font-size: 15px;
            max-width: 150px;
        }

        #message {
            position: fixed;
            border: inset 4px grey;
            top: 2px;
            right: 2px;
            background-color: #333;
            color: white;
            font-size: 16px;
            display: none;
            text-align: left;
            padding: 5px;
        }

        .bord1px {
            border: solid 1px grey;
            margin: 4px;
            padding: 2px;
            border-radius: 4px;
        }

        #mode,
        #option,
        .les_select {
            display: flex;
            justify-content: space-between;
            font-size: 20px;
            text-align: center;
        }

        #mode,
        #option,
        .les_select {
            background-color: #ddd;
            border-radius: 5px;
            margin: 4px;
            padding: 3px;
        }

        .boutons {
            display: inline-flex;
        }

        .TitZone {
            font-size: 12px;
            font-style: italic;
            font-weight: bold;
        }

        .minmax {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .minmax div {
            margin-right: 5px;
            margin-left: 5px;
        }

        .bouton_curseur,
        .les_select {
            display: flex;
            justify-content: space-between;
            font-size: 20px;
            text-align: center;
            margin: 4px;
            padding: 4px;
            border: 2px inset grey;
            background-color: rgba(30, 30, 30, 0.3);
            color: white;
            border-radius: 5px;
        }

        .slideTriac {
            width: 100%;
            position: relative;
            display: table;
        }

        .slideTriacIn {
            display: table-row;
            width: 100%;
        }

        .Tcell1 {
            display: table-cell;
            width: 35%;
            text-align: right;
        }

        .Tcell2 {
            display: table-cell;
            width: 60%;
            text-align: right;
        }

        .Tcell3 {
            display: table-cell;
            width: 5%;
            text-align: left;
            padding-left: 10px;
        }

        svg {
            border: 2px inset grey;
            border-radius: 5px;
            width: 100%;
        }
    </style>
    <title>Actions</title>
</head>

<body onLoad="SetHautBas();Init();" onmouseup="mouseClick=false;">
    <div class="cadre">
        <div id="lesOnglets"></div>
        <h4>Planning des Routages <small>(suivant <span id="nomSondeMobile">sonde Maison</span>)</small></h4>
        <h5 id="TitrTriac">Routage via Triac</h5>
        <div class="form">
            <div id="planning0" class="planning"></div>
        </div>
        <h5>Routage via Relais</h5>
        <div class="form">
            <div id="plannings"></div>
        </div>
        <div id="commun">
            <br>
            <h4>Paramètres communs aux Actions</h4>
            <div id="CACSI">
                <div>Réactivité si CACSI <span class="fsize10">Augmentée pour les valeurs de puissance inférieures au
                        seuil Pw. Pour tous les SSR ou Triac.</span></div>
                <div class="form2">
                    <div class="ligne source">
                        <label for="CACSI1" style="text-align:right;">Pas d'augmentation</label>
                        <input type="radio" name="ReacCACSI" id="CACSI1" value="1" checked>
                        <label for="CACSI2" style="text-align:right;">Réactivité x 2</label>
                        <input type="radio" name="ReacCACSI" id="CACSI2" value="2">
                        <label for="CACSI4" style="text-align:right;">Réactivité x 4</label>
                        <input type="radio" name="ReacCACSI" id="CACSI4" value="4">
                        <label for="CACSI8" style="text-align:right;">Réactivité x 8</label>
                        <input type="radio" name="ReacCACSI" id="CACSI8" value="8">
                    </div>
                </div>
            </div>
            <div id="Freq_PWM">
                <div>Fréquence signaux PWM <span class="fsize10"> Nécessite un Reset de l'ESP32</span></div>
                <div class="form2">
                    <div class="ligne source">
                        <label for="Fpwm5" style="text-align:right;">5 Hz</label>
                        <input type="radio" name="Fpwm" id="Fpwm5" value="5">
                        <label for="Fpwm50" style="text-align:right;">50 Hz</label>
                        <input type="radio" name="Fpwm" id="Fpwm50" value="50">
                        <label for="Fpwm500" style="text-align:right;">500 Hz</label>
                        <input type="radio" name="Fpwm" id="Fpwm500" value="500" checked>
                        <label for="Fpwm5000" style="text-align:right;">5000 Hz</label>
                        <input type="radio" name="Fpwm" id="Fpwm5000" value="5000">
                        <label for="Fpwm50000" style="text-align:right;">50000 Hz</label>
                        <input type="radio" name="Fpwm" id="Fpwm50000" value="50000">
                    </div>
                </div>
            </div>
        </div>

        <div id="BoutonsBas">
            <br><input class="bouton" type="button" onclick="SendValues();" value="Sauvegarder">
            <div class="lds-dual-ring" id="attente"></div>
            <input class="bouton" type="button" onclick="Reset();" value="ESP32 Reset">
        </div>
    </div>
    <div id="message"></div><br>
    <div id="pied"></div>
    <script src="/ParaRouteurJS"></script>
    <script src="/ActionsJS1"></script>
    <script src="/ActionsJS2"></script>
    <script src="/ActionsJS3"></script>
    <script src="/ActionsJS4"></script>
    
</body>

</html>
)====";