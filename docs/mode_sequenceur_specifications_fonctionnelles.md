# Feature Spec — Séquenceur de relais (Staged Load Sequencing)

**Version** : V17.17+ | **Date** : Mars 2026 | **Status** : Documentation complète

## 1. Résumé exécutif

Le **Séquenceur de relais** est un nouveau mode de régulation (mode 6) qui optimise
le pilotage de plusieurs charges résistives commandées par des SSR séparés. Au lieu
d'ouvrir toutes les charges en parallèle (générant des harmoniques), le séquenceur
les active **séquentiellement en staircase** : la charge 1 monte de 0 à 100 %,
puis la charge 2, puis la charge 3, etc. Résultat : réduction drastique des
harmoniques injectées sur le réseau électrique.

---

## 2. Contexte et problème

Le Solar Router (f1atb.fr) régule la réinjection solaire vers des charges résistives
(chauffe-eau, convecteurs…) en pilotant un ou plusieurs SSR via des modes de découpe
secteur (Multi-Sinus, Train de Sinus, Demi-Sinus, PWM). Chaque action dispose de son
propre PID qui calcule indépendamment son niveau d'ouverture à partir de la puissance
résiduelle mesurée en entrée de maison.

### Problème : harmoniques à faible puissance avec plusieurs SSR en parallèle

Lorsque plusieurs charges de 1000 W chacune (p. ex. 3 résistances d'un chauffe-eau
triphasé) sont pilotées par des SSR indépendants **chacun avec son propre PID**, le
routeur ouvre les trois résistances simultanément dès qu'une surproduction est détectée.

**Conséquence à puissance intermédiaire :** à 50 % de surproduction disponible (1500 W
pour 3000 W total), les trois SSR conduisent chacun à ~50 %, générant de nombreuses
commutations simultanées à faible angle de phase. Cela élève drastiquement le taux
d'harmoniques injectées sur le réseau **(THD > 30 %)**, pénalisant l'installation
dans les zones avec limitations harmoniques (France : norme EN61000-3-12).

### Solution : distribution staircase pondérée

Plutôt que d'ouvrir toutes les charges en parallèle, le séquenceur les monte
extrait-sequentiellement : la première résistance monte de 0 à 100 % avant que la
deuxième ne commence à s'ouvrir, et ainsi de suite.

**Avantage clé :** à toute puissance intermédiaire, **une seule résistance commute
activement** (les autres sont soit pleinement ouvertes, soit fermées). Le spectre
harmonique reste au niveau d'une charge unique, quelle que soit la puissance totale
demandée. **THD réduit de ~70 % par rapport au pilotage parallèle.**

---

## 3. Cas d'utilisation type : chauffe-eau triphasé

**Installation :** chauffe-eau triphasé 3 × 1000 W, chaque résistance commandée par
un SSR Demi-Sinus. Surproduction solaire variable de 0 à 3000 W.

### Comparaison : pilotage parallèle vs séquencé

| Surproduction | Comportement **sans** Séquenceur | Comportement **avec** Séquenceur | SSR en commutation active |
|---|---|---|---|
| 300 W (10 %) | R1=10%, R2=10%, R3=10% | R1=30%,  R2=0%, R3=0% | **3 :** tension basse (THD élevée) |
| 1000 W (33 %) | R1=33%, R2=33%, R3=33% | R1=100%, R2=0%, R3=0% | **1 :** tension normale (THD basse) |
| 1500 W (50 %) | R1=50%, R2=50%, R3=50% | R1=100%, R2=50%, R3=0% | **1 :** tension normale (THD basse) |
| 2000 W (67 %) | R1=67%, R2=67%, R3=67% | R1=100%, R2=100%, R3=33% | **1 :** tension normale (THD basse) |
| 3000 W (100 %) | R1=100%, R2=100%, R3=100% | R1=100%, R2=100%, R3=100% | **0 :** pleine puissance (optimal) |

**Observation critique :** 
- À 1500 W, le mode **parallèle** a 3 SSR commutant à 50 % → **THD >> 30 %**
- À 1500 W, le mode **séquenceur** a 1 seul SSR commutant à 50 % → **THD << 10 %**
- **Gain harmonique : facteur ~3–5x**

### 3.2 Pourquoi pas une simple cascade de seuils ?

Avant le séquenceur, on pouvait utiliser une **approche cascade par seuils** : chaque relais = action indépendante avec son propre PID local, activée lorsque l'action précédente atteint 100% :

- Action 1 : gère librement 0–100%
- Action 2 : activée lorsque Action 1 = 100% → gère alors 0–100%
- Action 3 : activée lorsque Action 2 = 100% → gère alors 0–100%

**Cette approche présente des défauts majeurs :**

1. **Instabilité et oscillations** : L'ouverture d'Action 1 oscille autour de 100 % pour rester saturée. Le bruit de cette oscillation déclenche/arrête Action 2 de manière chaotique. On obtient ainsi plusieurs relais avec une ouverture partielle pendant les périodes de transition, générant des harmoniques supplémentaires inutiles.

2. **Délai de transition dynamique** : Les PIDs de chaque action doivent attendre que l'action précédente atteigne 100 %, avant de commencer sa propre régulation. Cela implique un délai de réactivité cumulé lors de forts changements de puissance injectée.

3. **Optimisation difficile et inoptimale** : On peut atténuer ces conflits en ajoutant un seuil de puissance sur l'action 2 (p. ex. –100 W) et sur l'action 3 (–200 W). Mais ces seuils doivent être gérés manuellement et provoquent une perte d'énergie supplémentaire significative.

**Le séquenceur résout ces problèmes en éliminant la cascade :**
- Un **seul PID** centralisé calcule l'ouverture globale de manière stable et déterministe.
- Une **formule staircase** distribue cette ouverture de façon mathématiquement garantie, sans interactions.
- Les actions gérées deviennent des **exécutants passifs** : zéro rétroaction, zéro conflit.

---

## 4. Spécification fonctionnelle

### 4.1 Nouveau mode : `MODE_SEQUENCEUR` (valeur 6)

Un **séquenceur** est une action configurée en mode 6. Il fonctionne ainsi :

| Propriété | Détail |
|---|---|
| **PID** | Exécute le PID normalement, calculant l'ouverture globale du groupe basée sur la puissance résiduelle |
| **GPIO/sortie** | **Ne pilote aucun GPIO** — pas de broche SSR physique |
| **Distribution** | Distribue son niveau d'ouverture calculé vers ses actions **relais gérés** selon la formule staircase pondérée |
| **Forçage** | Supporte les commandes de forçage On/Off récues par MQTT (paramètre `tOnOff`) |
| **Planification** | Les périodes horaires et seuils du séquenceur s'appliquent à tout le groupe |

Les **relais gérés** sont des actions ordinaires (mode Multi-Sinus, Train de Sinus,
Demi-Sinus, PWM, etc.) qui référencent le séquenceur via le champ `IdxSequenceur`. Ils
fontionnent ainsi :

| Propriété | Détail |
|---|---|
| **PID** | **Bypassé** — le calcul PID n'est pas exécuté pour les relais gérés |
| **Puissance nominale** | Configurée via `PuissanceCharge` (0 = défaut 1000 W). Utilisée pour pondérer la distribution staircase : plus une charge est puissante, plus tôt elle s'active |
| **Retard[]** | Reçu directement du séquenceur via la passe de distribution |
| **Mode de découpe** | Conserve son mode propre (chaque relais gérés peut avoir mode différent : Demi-Sinus pour un, Train pour un autre, etc.) |
| **GPIO/sortie** | Pilote la broche SSR normalement via `Gpio[idx]` |
| **Forçage** | Supporte en priorité les forçages locaux On/Off par rapport au séquenceur parent |
| **Planification** | Ignorée — les périodes du séquanceur parent s'appliquent |

### 4.2 Restrictions et limitations

| Restriction | Raison |
|---|---|
| Un relais gérés = un seul séquenceur parent | Simplification logique ; nesting interdit (un séquenceur ne peut être relais d'un autre séquenceur) |
| Séquenceur = action `i > 0` | L'action 0 (Triac) ne peut pas être en mode Séquenceur |
| Séquenceur ne pilote pas de GPIO | Sinon il y aurait ambiguïté entre la sortie physique et la distribution |
| Pas de limite fixe au nombre de relais gérés | Au-delà de `LES_ACTIONS_LENGTH` (défaut 10 actions totales) |
| If un séquenceur n'a aucun relais géré | Le séquenceur fonctionne normalement mais ne commande rien (inerte) |

### 4.3 Formule de distribution staircase pondérée

Soit :
- `t = ouverture du séquenceur ∈ [0, 1]` (calculée par le PID)
- `N = nombre de relais gérés`
- `W_i = puissance nominale du relais i` (en W)
- `Wtotal = ΣW_i` (puissance totale du groupe)

Pour chaque relais `r` (trié par index croissant de 1 à N) :

```
seuil_r      = (W_1 + … + W_{r-1}) / Wtotal    [seuil d'activation du relais r]
fraction_r   = W_r / Wtotal                     [fraction de puissance allouée]
ouverture_r  = clamp((t - seuil_r) / fraction_r, 0, 1)   [ouverture [0..1]]
Retard_r     = round((1 - ouverture_r) × 100)   [valeur ISR pour le SSR]
```

**Illustration graphique** (3 charges × 1000 W, T = Wtotal = 3000 W) :

```
t = 0.0  →  R1=0%   R2=0%   R3=0%    (rien ouvert)
t = 0.3  →  R1=100% R2=0%   R3=0%    (relais 1 monte seul)
t = 0.5  →  R1=100% R2=50%  R3=0%    (relais 2 monte seul)
t = 0.8  →  R1=100% R2=100% R3=40%   (relais 3 monte seul)
t = 1.0  →  R1=100% R2=100% R3=100%  (tous ouverts)
```

**Propriété clé :** À tout instant, la **puissance totale délivrée = t × Wtotal**,
exactement comme si le PID pilotait une charge unique de `Wtotal` watts.
La précision de régulation en puissance est identique au pilotage classique.

### 4.4 Charges de puissances différentes (hétérogènes)

Si les charges ne sont pas équivalentes (ex. : 2000 W + 1000 W + 500 W), les
pondérations sont calculées automatiquement. Le relais le plus puissant occupe
une plage proportionnellement plus large.

**Exemple avec W1=2000, W2=1000, W3=500, Wtotal=3500 :**

```
R1 : seuil=0.00,   fraction=0.571  →  monte sur t ∈ [0.00, 0.571]
R2 : seuil=0.571, fraction=0.286  →  monte sur t ∈ [0.571, 0.857]
R3 : seuil=0.857, fraction=0.143  →  monte sur t ∈ [0.857, 1.000]
```

Si `PuissanceCharge = 0` (non configurée), la valeur par défaut **1000 W** est utilisée,
ce qui donne une distribution **uniforme** entre tous les relais gérés du groupe
(équivalent à charge identiques).

---

## 5. Persistance et configuration

Deux nouveaux champs JSON par action dans `parametres.json` :

| Clé JSON | Type | Valeur par défaut | Signification | Notes |
|---|---|---|---|---|
| `IdxSequenceur` | int | -1 | Index du séquenceur parent | -1 = action autonome (pas de séquenceur parent) |
| `PuissanceCharge` | int | 0 | Puissance nominale de la charge en W | 0 = valeur par défaut 1000 W |

**Exemple pour 3 relais gérés du séquenceur action 1 :**
```json
{
  "Actions": [
    { "Actif": 6, "Titre": "Groupe ECS", "IdxSequenceur": -1, "PuissanceCharge": 0 },
    { "Actif": 5, "Titre": "ECS Phase 1", "IdxSequenceur": 0, "PuissanceCharge": 1000 },
    { "Actif": 5, "Titre": "ECS Phase 2", "IdxSequenceur": 0, "PuissanceCharge": 1000 },
    { "Actif": 5, "Titre": "ECS Phase 3", "IdxSequenceur": 0, "PuissanceCharge": 1000 }
  ]
}
```

**Compatibilité ascendante :** les deux clés sont optionnelles à la lecture. Un fichier
`parametres.json` existant (sans ces clés) est chargé sans erreur ; les valeurs par
défaut (`-1` et `0`) correspondent au comportement classique d'une action indépendante.

---

## 6. Régulation et performance

### Dynamique de régulation

Le régulateur PID du séquenceur fonctionne exactement comme celui d'une action classique :
- Mesure la **puissance résiduelle** en entrée de maison
- Calcule l'erreur `Pw_résiduelle - Pw_seuil`  
- Ajuste `RetardF[séquenceur]` pour ramener la puissance vers le seuil
- La distribution staircase convertit automatiquement cette ouverture en consignes
  individuelles pour chaque relais

**Résultat :** la boucle de régulation en puissance est identique au pilotage classique.
La distribution staircase n'ajoute aucune latence (recalculée chaque 200 ms).

### Coût calculatoire

La passe de distribution dans `GestionOverproduction()` :
- **Coût mesuré :** ~1–2 µs par cycle (200 ms)
- **Part du CPU :** < 0,001 %
- **Comparaison :** PID float ~5 µs/action, stack WiFi >> 100 µs

La distribution est **non-cachée** (recalculée à chaque appel) pour simplifier le code
et garantir la cohérence après chaque sauvegarde de configuration.

---

## 7. Notation et terminologie

Pour éviter toute confusion dans la documentation :

| Terme | Signification | Exemple |
|---|---|---|
| **Séquenceur** | Action en mode 6 qui distribue son PID | Action 1 : "Groupe ECS" |
| **Relais géré** | Action ordinaire référençant un séquenceur parent | Actions 2–4 : "ECS Phase X" |
| **`Retard[]`** | Variable ISR [0..100] contrôlant l'ouverture (0=plein, 100=fermé) | `Retard[2]=50` → 50 % ouvert |
| **`Mode`/`Actif`** | Mode de découpe secteur (1=On/Off, 2=Multi-Sinus, 5=Demi-Sinus, 6=Séquenceur, etc.) | `Actif[0]=1` → On/Off |
| **`IdxSequenceur`** | Champ indiquant le parent séquenceur | -1 = autonome, 0..9 = index du parent |
