# Implementation Guide — Séquenceur de relais

**Version** : V17.17+ | **Date** : Mars 2026 | **Auteur** : F1ATB

## 1. Vue d'ensemble technique

Le Séquenceur de relais introduit un nouveau mode de régulation (`MODE_SEQUENCEUR = 6`) qui permet
à une action "séquenceur" (sans GPIO) de distribuer son ouverture PID vers plusieurs
actions "relais gérés" selon une formule de staircase pondérée. 

**Périmètre des modifications :**
- **5 fichiers modifiés**, ~200 lignes de code au total
- **Aucune modification d'architecture**, pleine compatibilité ascendante
- **Coût mémoire :** +8 bytes par action (2 int de 4 bytes)
- **Coût CPU :** ~1–2 µs par cycle (200 ms), < 0,001 % du CPU core 1

---

## 2. Fichiers modifiés

### 1. `Actions.h` — Deux nouveaux champs dans la classe `Action`

```cpp
// Relais séquencé (MODE_SEQUENCEUR=6) : un séquenceur PID distribue son ouverture sur ses relais gérés
// en découpant la plage [0..1] proportionnellement aux puissances nominales des charges.
int IdxSequenceur;   // -1 → action autonome (séquenceur ou action classique)
                      // ≥0 → index du séquenceur dont ce relais séquencé reçoit son Retard[]
int PuissanceCharge;  // Puissance nominale de la charge pilotée par ce relais séquencé (W).
                      // Utilisée pour pondérer la distribution. 0 → valeur par défaut 1000 W
```

Les champs sont ajoutés après les membres existants `bool On, PID; float H_Ouvre;`,
sans modifier l'alignement ni l'ordre des membres existants.

---

### 2. `Actions.cpp` — Initialisation dans le constructeur

```cpp
IdxSequenceur = -1;   // Autonome par défaut
PuissanceCharge = 0;   // Puissance non définie (1000 W par défaut à l'usage)
```

Ajouté après les initialisations existantes de `ExtOuvert`. Strictement additif.

En complément, `Actions.cpp` applique une neutralisation explicite du mode séquenceur
au niveau des sorties locales (GPIO/appels externes) pour garantir qu'une action
`MODE_SEQUENCEUR` reste purement logique, même si des GPIO/appels externes étaient
préalablement configurés.

---

### 3. `Solar-Router-F1ATB.ino` — Trois ajouts

#### 3a. Définition de la constante `MODE_SEQUENCEUR`

```cpp
#define MODE_INACTIF 0
#define MODE_DECOUPE_ONOFF 1
#define MODE_MULTISINUS 2
#define MODE_TRAINSINUS 3
#define MODE_PWM 4
#define MODE_DEMISINUS 5
#define MODE_SEQUENCEUR 6  // Nouveau : Séquenceur de relais (PID virtuel sans GPIO)
```

Ajouté juste après `MODE_DEMISINUS` dans la séquence naturelle des modes (après ligne ~100).

#### 3b. Helper `AppliquerRetard(int idx)` — factorisation de la logique ISR

Avant `GestionOverproduction()`, une nouvelle fonction centralise la mise à jour des
variables ISR (`PulseOn`, `PulseTotal`, `RelaisOn`/`Arreter`) en fonction du mode :

```cpp
void AppliquerRetard(int idx) {
  if (Retard[idx] == 100) {
    LesActions[idx].Arreter();
    PulseOn[idx] = 0;
  } else {
    switch (Actif[idx]) {
      case MODE_DECOUPE_ONOFF: if (idx > 0) LesActions[idx].RelaisOn(); break;
      case MODE_MULTISINUS:    /* lookup table */ break;
      case MODE_TRAINSINUS:    /* + testTrame */ break;
      case MODE_PWM:           /* ledcWrite */   break;
      case MODE_DEMISINUS:     /* phase */       break;
    }
  }
}
```

Ce helper évite de dupliquer la logique entre la boucle principale (actions standalone) et
la passe de distribution (actions séquencées), cf section suivante.


#### 3c. Passe de distribution dans `GestionOverproduction()`

Deux points d'intégration dans la boucle principale existante :

**① Skip relais séquencé** (ajout d'une ligne) :
```cpp
if (LesActions[i].IdxSequenceur >= 0) continue;  // Relais séquencé : skip PID
```
Les relais gérés ne calculent pas leur propre PID. Leur `Retard[]` sera fixé par la passe
de distribution.

**② Passe de distribution** (après la boucle principale, avant `LissageLong`) :
```cpp
for (int iSeq = 0; iSeq < NbActions; iSeq++) {
  if (LesActions[iSeq].Actif != MODE_SEQUENCEUR) continue;
  // ... collecte des relais gérés ...
  float t = 1.0f - (RetardF[iSeq] / 100.0f);  // ouverture groupe [0..1]
  float puissCumul = 0.0f;
  for (int r = 0; r < nbRelaisGeres; r++) {
    float seuil     = puissCumul / float(puissTotale);
    float fracPuiss = float(puissRelais[r]) / float(puissTotale);
    float ouverture = constrain((t - seuil) / fracPuiss, 0.0f, 1.0f);
    puissCumul     += puissRelais[r];
    RetardF[j]      = (1.0f - ouverture) * 100.0f;
    Retard[j]       = round(RetardF[j]);
    AppliquerRetard(j);
  }
}
```

**Forçage de relais gérés :** chaque relais géré supporte un forçage local
via `LesActions[j].tOnOff` (reçu par MQTT) qui **prime sur la distribution
du séquenceur**. Si `tOnOff > 0` (forçage On), on applique `ForceOuvre` ;
si `tOnOff < 0` (forçage Off), on force `Retard[j]=100` (arrêt).

**Log Telnet relais géré :** conditionné à `RetardVx == j` (même mécanisme que le log
séquenceur), il affiche (all floats avec 2 décimales) :
```
[relais 2 <- seq 0]  GroupeOuv= 50.00%  SeuilDemarrage= 33.33%  FractionGroupe= 33.33%  RelaisOuv= 50.00%  Retard= 50
```
Cela permet de diagnostiquer la distribution en temps réel sur Telnet port 23.

**Neutralisation des GPIO de séquenceurs :**
`Solar-Router-F1ATB.ino` inclut aussi une neutralisation explicite du séquenceur
dans les chemins d'initialisation/pilotage bas niveau (ISR et initialisation GPIO),
afin d'éviter toute commutation physique sur l'action parent.

---

### 4. `Stockage.ino` — Sérialisation JSON

#### Désérialisation (lecture du fichier `parametres.json`)
```cpp
LesActions[iAct].IdxSequenceur  = !obj["IdxSequenceur"].isNull()  ? (int)obj["IdxSequenceur"]  : -1;
LesActions[iAct].PuissanceCharge = !obj["PuissanceCharge"].isNull() ? (int)obj["PuissanceCharge"] : 0;
```

#### Sérialisation (écriture)
```cpp
obj["IdxSequenceur"]  = LesActions[iAct].IdxSequenceur;
obj["PuissanceCharge"] = LesActions[iAct].PuissanceCharge;
```

**Compatibilité ascendante :** le check `isNull()` avant lecture garantit que tout
fichier de configuration existant (sans ces clés) est chargé sans erreur. Les valeurs
par défaut (-1 / 0) correspondent au comportement d'une action classique.

---

### 5. `JS_Actions.h` — Interface web et logique client

Quatre zones modifiées dans le JavaScript embarqué :

#### 5a. `CreerAction(NumAction, Titre)`
Ajout des champs `IdxSequenceur: -1, PuissanceCharge: 0` dans l'objet par défaut
(nouvelles actions initialisées en "mode autonome").

#### 5b. `TracePlanning(iAct)`
1. **Radio mode 6** : Ajout du bouton radio *Séquenceur de relais* (mode 6) avec un tooltip
   descriptif : "Distribue la puissance séquentiellement sur plusieurs charges..."
2. **Panneau informatif séquenceur** : `<div id='groupeSequenceur${iAct}'>` affiché uniquement
   en mode 6 (texte : "Ce mode distribue l'ouverture PID vers les relais gérés. Aucune sortie
   GPIO directe.")
3. **Panneau relais séquencé** : `<div id='relaisSequence${iAct}'>` masqué par défaut, contient :
   - Select `selectSequenceur${iAct}` : liste les actions en mode 6 disponibles
   - Input `puissanceCharge${iAct}` : champ numérique pour la puissance nominale en W
4. **Restauration depuis `F.Actions`** : après construction HTML, les valeurs `IdxSequenceur`
   et `PuissanceCharge` sont restaurées dans les contrôles si l'action existait.

#### 5c. `checkDisabled()`
Logique complexe de visibilité conditionnelle :

```javascript
const estSequenceur = (actif === 6 && iAct > 0);
const estOnOff = (actif === 1 && iAct > 0);
const estRelaisSequence = (selectSequenceur && selectSequenceur.value != "-1");
```

Puis masquage/affichage en fonction :
- `groupeSequenceur` : affiché ssi `estSequenceur`
- `relaisSequence` : affiché ssi au moins un séquenceur existe ET `!estSequenceur` ET `!estOnOff`
- `SelectPin` : masqué ssi `estSequenceur` (pas besoin de GPIO physique)
- `SelectOut` : masqué ssi `estSequenceur OR estOnOffExterne`
- `PIDbox`, `visu`, planning : masqués ssi `estRelaisSequence` (gérés par le séquenceur parent)
- Mode 6 désactivé sur action 0 (Triac)
- Reconstruction du select `selectSequenceur` à chaque changement via boucle DOM pour lister
  les séquenceurs existants

#### 5d. `Send_Values()`
Collecte des deux nouveaux champs avant envoi :
```javascript
const selectSequenceur = GID("selectSequenceur" + iAct);
action.IdxSequenceur = selectSequenceur ? (parseInt(selectSequenceur.value, 10) || -1) : -1;
const champPuissance = GID("puissanceCharge" + iAct);
action.PuissanceCharge = champPuissance ? (parseInt(champPuissance.value, 10) || 0) : 0;
```

Boucle des modes extensible à 0–6 (au lieu de 0–5).
Logique d'effacement : un séquenceur (`Actif===6`) est conservé même si
`selectPin===0` (pas de GPIO) — contrairement aux autres modes.

---

## 3. Impact sur les fonctions existantes

| Fonction | Impact |
|---|---|
| `GestionIT_10ms()` (ISR) | **Aucun impact fonctionnel majeur.** Neutralisation explicite du séquenceur en ISR. |
| `handleAjax_etatActions()` | **Aucun.** La boucle `for (int i = 0; i < NbActions; i++)` inclut naturellement le séquenceur (`Actif > 0`) et les relais gérés. `Retard[i]` est à jour pour tous. |
| `SendDataToHomeAssistant()` | **Aucun.** Même boucle, même condition. |
| `handleAjaxHisto48h/10mn()` | **Aucun.** `tab_histo_ouverture` est rempli pour toutes les actions actives. |
| `InitGPIOs()` | **Aucun impact fonctionnel majeur.** Neutralisation explicite des GPIO du séquenceur à l'initialisation des sorties. |
| `Stockage` lecture/écriture | Étendu avec les deux nouveaux champs, rétrocompatible. |

---

## 4. Tests et validation

### Test 1 : Régression — actions classiques inchangées

1. Configurer action 1 en Multi-Sinus, GPIO 4, seuil 50 W, Ki=10
2. Configurer action 2 en Demi-Sinus, GPIO 5, seuil 100 W, Ki=10
3. Vérifier :
   - Les deux actions réagissent normalement à la puissance résiduelle
   - `IdxSequenceur = -1`, `PuissanceCharge = 0` dans `parametres.json`
   - Aucune modification de `Retard[]` vs V17.15

### Test 2 : Groupe homogène (3 × 1000 W)

1. Action 0 : Séquenceur (mode 6, seuil 50 W, Ki=10)
2. Actions 1–3 : Demi-Sinus (mode 5), GPIO 4/5/18, `IdxSequenceur=0`, `PuissanceCharge=1000`
3. Injecter 1500 W de surproduction → vérifier :
   - Séquenceur retard = 50 (au seuil)
   - Relais 1 `Retard[1]=0` (100 % ouvert)
   - Relais 2 `Retard[2]=50` (50 % ouvert, en commutation active)
   - Relais 3 `Retard[3]=100` (fermé)

### Test 3 : Groupe hétérogène (2000 W + 1000 W + 500 W)

1. Action 0 : Séquenceur (mode 6, seuil 50 W)
2. Actions 1–3 : Demi-Sinus, `IdxSequenceur=0`, `PuissanceCharge=[2000, 1000, 500]`
3. Injecter 1750 W → vérifier distribution pondérée correcte

### Test 4 : Persistance

1. Configurer séquenceur + relais gérés, sauvegarder
2. Arrêter le routeur, vérifier `parametres.json` contient les nouveaux champs
3. Redémarrer → vérifier UI affiche la config restaurée

### Test 5 : Log Telnet

```
Telnet localhost 23
> RetardVx 1
> (injecter surproduction)
[relais 1 <- seq 0]  GroupeOuv= 75.00% ...
> RetardVx -1
```

### Test 6 : MQTT et forçage

1. Publier `topic/Action_0/tOnOff` = `50` (forçage actif 50%)
2. Vérifier séquenceur s'ouvre et force ses relais gérés
3. Publier `-50` (forçage arrêt) → tous les relais passent à 0%
4. Vérifier topics publiés correctement
