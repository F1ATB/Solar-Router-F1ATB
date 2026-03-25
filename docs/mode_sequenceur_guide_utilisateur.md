# Guide Utilisateur — Séquenceur de relais

**Version** : V17.17+ | **Mise à jour** : Mars 2026 | **Public** : Utilisateurs finaux / Installateurs

## 1. Qu'est-ce que le Séquenceur de relais ?

Le **Séquenceur de relais** est un mode de régulation avancé qui optimise le pilotage
de plusieurs charges résistives (chauffe-eau, convecteurs, radiateurs…) pilotées par des SSR.

Au lieu de **commander toutes les charges en parallèle** (ce qui génère des harmoniques),
le séquenceur les active **séquentiellement** : la charge 1 monte de 0 à 100 %, puis
la charge 2 monte, puis la charge 3, etc.

**Résultat:**
- À puissance intermédiaire, **une seule charge commute activement** (les autres sont soit
  pleine ouverture, soit arrêt)
- **Taux d'harmoniques réduit de 70–80 %** comparé au pilotage parallèle
- **Régulation précise en puissance** : identique au pilotage classique
- **Transparent d'un point de vue utilisateur** : aucune modification du comportement observable

---

## 2. Quand utiliser le Séquenceur de relais ?

### Cas typiques (recommandé)

✅ **Chauffe-eau électrique triphasé** 3 × 1000 W (ou plus)
✅ **Batteries résistances** (radiateurs électriques avec éléments séparés)
✅ **Installation triphasée** : une résistance par phase de 1000–5000 W
✅ **Zone avec normes harmoniques strictes** : France (EN 61000-3-12), Allemagne (TR-BT 004), etc.

### Cas non applicables

❌ **Charge unique** (un seul SSR) : Séquenceur inutile, pilotage classique suffit
❌ **Charges très faibles** : < 500 W par élément (marges de répartition trop faibles)
❌ **Thyristors (Triac)** : mode Séquenceur non disponible sur l'action 0
❌ **Charge inductive** : le séquenceur n'a pas de contrôle harmonique pour les moteurs

---

## 3. Configuration pas à pas

La configuration se fait entièrement depuis la page **Actions** de l'interface web du routeur.

### Étape 1 — Créer l'action séquenceur

Le séquenceur est un **régulateur virtuel** : il contient le PID du groupe mais
ne pilote aucun GPIO physique (pas de broche SSR).

**Procédure :**

1. Dans la page **Actions**, cliquez sur le bouton **+** en bas pour créer une nouvelle action.
   (Ou éditez une action existante inutilisée, ex. : action 5–9 si vous en avez peu)

2. Remplissez les champs :
   - **Titre** : ex. "Groupe ECS" ou "Séquenceur Chauffage"
   - **Mode** : sélectionnez **Séquenceur de relais** (radio mode 6)
   - **Sortie GPIO** : laissez sur "pas choisi" ← **important** (aucune broche physique)

3. Configurez les **paramètres de régulation** exactement comme une action classique,
   en raisonnant sur la **puissance totale du groupe** :
   - **Seuils de puissance** (`Vmin`/`Vmax`) : définissent quand le groupe s'active
   - **Coefficients PID** : utilisez `Ki=10` par défaut (comme une action Single-Rate)
   - **Périodes horaires** : plages d'activation du groupe
   - **Exemple :** pour 3 × 1000 W = 3000 W total :
     - Seuil bas : 100 W (activation du groupe)
     - Seuil haut : 2500 W (limitation haute)
     - Ki : 10 (réaction modérée)

4. Cliquez **Sauvegarder**.

⚠️ **Remarque technique :** Le Séquenceur de relais (mode 6) n'est pas disponible
sur l'action 0 (Triac secteur).

### Étape 2 — Configurer les actions relais séquencés

Pour chaque **charge physique** pilotée par le séquenceur :

1. Créez ou éditez une action relais ordinaire (ex. : action 1, 2, 3 pour les phases).

2. Remplissez les champs de base :
   - **Titre** : ex. "ECS Phase 1", "ECS Phase 2", "ECS Phase 3"
   - **Mode** : choisissez le mode de découpe souhaité
     - Compatible avec tous les modes **Demi-Sinus**, **Multi-Sinus**, **Train de Sinus**, **PWM**
     - Vous pouvez mélanger les modes par charge (ex. : phase 1 en Demi-Sinus, phase 2 en Multi-Sinus)
   - **Sortie GPIO** : sélectionnez la broche connectée au SSR (ex. GPIO 4, 5, 18)

3. **Important :** Dans le panneau **Relais séquencé** (qui apparaît automatiquement pour tous les relais compatibles
dès qu'au moins une action Séquenceur existe les autres actions) :
   - **Séquenceur** : dans la liste déroulante, sélectionnez l'action créée à l'étape 1
     (ex. : "Groupe ECS")
   - **Puissance de la charge (W)** : entrez la puissance nominale de cette charge
     - Exemple : 1000 W pour une résistance 1000 W
     - Laissez à **0** si toutes les charges sont identiques (distribution uniforme)

4. ⚠️ **Les périodes horaires et options de PID sont cachés pour cette action** — elles sont ignorées au runtime
   (le séquenceur parent les gère pour tout le groupe).

5. Cliquez **Sauvegarder**.

**Répétez cette procédure pour chaque charge du groupe.**

### Étape 3 — Exemple complet : chauffe-eau triphasé 3 × 1000 W

#### Configuration à créer

| Action | Titre | Mode | GPIO | Séquenceur | Puissance | Seuil bas | Ki |
|---|---|---|---|---|---|---|---|
| **0** | Triac (inactif) | Inactif | — | — | — | — | — |
| **1** | Groupe ECS | **Séquenceur de relais** | **— (aucun)** | **—** | **—** | **100 W** | **10** |
| **2** | ECS Phase 1 | Demi-Sinus | GPIO 4 | Action 1 | **1000 W** | *(ignoré)* | *(ignoré)* |
| **3** | ECS Phase 2 | Demi-Sinus | GPIO 5 | Action 1 | **1000 W** | *(ignoré)* | *(ignoré)* |
| **4** | ECS Phase 3 | Demi-Sinus | GPIO 18 | Action 1 | **1000 W** | *(ignoré)* | *(ignoré)* |

#### Comportement observé

Avec une surproduction progressive :

1. **0–100 W** : tous fermés (en attente du seuil de 100 W)
2. **100–1100 W** : phase 1 seule s'ouvre progressivement (0→100 %)
3. **1100–2100 W** : phase 1 à 100 %, phase 2 s'ouvre (0→100 %)
4. **2100–3000 W** : phases 1–2 à 100 %, phase 3 s'ouvre (0→100 %)
5. **> 3000 W** : toutes à 100 % (max du groupe atteint)

**À 1500 W** : Phase 1 à 100%, Phase 2 à 50% (en commutation), Phase 3 fermée
→ **Harmoniques réduites** car une seule phase commute activement.

#### Sauvegarder la configuration

Cliquez **Sauvegarder** en bas de page → la configuration est écrite dans `parametres.json`

---

## 4. Interface web — Changements visibles

### Page Actions

#### Pour le séquenceur (action en mode 6)

- **Panneau séquenceur** visible : texte informatif
  > *Ce mode distribue l'ouverture PID vers les relais gérés. Aucune sortie GPIO directe.*
- **Sélecteur GPIO** : masqué (pas d'utilité, le séquenceur n'a pas de broche physique)
- **Planification et PID** : **complètement visibles et configurables**
  (c'est le cœur du séquenceur)

#### Pour les actions relais gérés (avec séquenceur parent)

- **Panneau relais séquencé** visible, contenant :
  - Sélecteur déroulant "Séquenceur" → liste les actions en mode 6
  - Champ numérique "Puissance de la charge (W)" → saisir la puissance nominale
- **Sélecteur GPIO** : **visible et actif** (à vous de le configurer)
- **Planification et PID** : masqués (ignorés au runtime, gérés par le séquenceur)
- **Mode de découpe** : visible et modifiable (choisir Demi-Sinus, Multi-Sinus, etc.)

### Page Accueil (supervision en temps réel)

Le tableau des actions affiche :
- **Séquenceur** : ouverture globale du groupe (ex. : 50 % = distribution à mi-plage)
- **Relais gérés** : ouverture calculée par la distribution (ex. : phase 1 = 100%, phase 2 = 50%)
- **Durée équivalente** (`H_Ouvre`) : cumulée pour chaque action individuellement

### MQTT / Home Assistant

Chaque action publie ses topics normalement :
- `routeur/Ouverture_Relais_N` : ouverture calculée pour l'action N
- `routeur/Actif_Relais_N` : état actif/inactif
- `routeur/Duree_Relais_N` : durée équivalente en heures

**Pour le séquenceur :** l'ouverture reflète la fraction de puissance totale demandée.
**Pour les relais gérés :** l'ouverture reflète la distribution staircase calculée.

---

## 5. Règles et limites opérationnelles

| Règle | Détail | Impact utilisateur |
|---|---|---|
| **Un groupe = un séquenceur** | Plusieurs séquenceurs indépendants possibles, mais chaque relais ne peut référencer qu'un seul séquenceur | Créer un séquenceur par groupe logique (ex. : Groupe ECS, Groupe Radiateurs) |
| **Séquenceur sans GPIO** | Le séquenceur ne doit pas avoir de GPIO configuré | Laisser "pas choisi" dans le sélecteur GPIO du séquenceur |
| **Planification du séquenceur s'applique au groupe** | Les périodes horaires du séquenceur s'appliquent à TOUS ses relais gérés | Configurer la planification une seule fois sur le séquenceur |
| **Relais gérés : planification ignorée** | La configuration PID/planification d'un relais géré est sauvegardée mais ignorée au runtime | Ne pas dépenser du temps à configurer ces champs pour les relais gérés |
| **Charges mixtes supportées** | Puissances nominales différentes (ex. : 2000 W + 1000 W + 500 W) sont OK | Entrer la vraie puissance pour chaque charge (sinon distribution inégale) |
| **Ordre de montée = ordre d'index** | La charge 1 monte avant la charge 2, puis 3, etc. | L'action 1 doit avoir le plus petit index pour être prioritaire |
| **Pas de nesting de séquenceurs** | Un séquenceur ne peut pas être relais géré d'un autre séquenceur | Chaque séquenceur est indépendant |
| **Forçage On/Off primaire sur relais gérés** | Un forçage local On/Off via MQTT prime sur la distribution du séquenceur | `topic/Action_2/tOnOff` peut forcer fermé même si le séquenceur demande ouverture |

---

## 6. Dépannage et diagnostic

### Symptôme : Les relais gérés ne réagissent pas du tout

**Checklist :**
1. ✓ Le séquenceur est bien en mode **Séquenceur de relais** (mode 6) ?
   → Vérifier page Actions, radio mode du séquenceur
2. ✓ Configuration **sauvegardée** ?
   → Cliquer le bouton Sauvegarder en bas de page
3. ✓ Routeur **redémarré** après la sauvegarde ?
   → Redémarrage nécessaire pour charger la new config
4. ✓ Chaque relais géré référence le bon séquenceur ?
   → Page Actions → relais géré → panneau "Relais séquencé" → sélecteur "Séquenceur" :
     doit afficher le titre du séquenceur (ex. "Groupe ECS")
5. ✓ GPIO de chaque relais géré configuré et valide ?
   → Sélecteur GPIO doit afficher GPIO 4, 5, etc. (pas "pas choisi")

**Si toujours bloqué :**
- Connecter en **Telnet** (port 23) et chercher logs d'erreur
- Vérifier `parametres.json` en accès direct LittleFS (si possible)

### Symptôme : Ouverture des relais gérés toujours à 0 %, même avec surproduction

**Checklist :**
1. ✓ Une **surproduction est-elle présente** ?
   → Vérifier page Accueil : colonne "Puissance" doit montrer valeur positive
2. ✓ Le **séquenceur a-t-il un seuil de puissance cohérent** ?
   → Ex. : seuil bas = 100 W, surproduction = 500 W → séquenceur devrait s'ouvrir
3. ✓ Le **séquenceur a-t-il une période horaire active** ?
   → Page Actions → séquenceur → plages horaires : au moins une tranche doit être active à l'heure courante
4. ✓ Le **séquenceur est-t-il masqué par une période OFF** ?
   → Éditeur les périodes et vérifier horaires
5. ✓ Le **filtre de seuil bas** du séquenceur est-il trop élevé ?
   → Réduire le seuil bas (ex. : 50 W) et tester

### Symptôme : Distribution inégale entre des charges supposées identiques

**Exemple problématique :**
- 3 charges : Phase 1, 2, 3
- Phase 1 s'ouvre parfois à 100 %, Phase 2 à 0 % → déséquilibre

**Checklist :**
1. ✓ Champ **Puissance** identique pour toutes les charges ?
   → Vérifier par image : toutes à 1000, ou toutes à 0 (laissé vide)
   → **Ne pas mélanger** 1000, 0, 1000 (confusion!)
2. ✓ L'ordre des actions est-il logique ?
   → Plus petit indice = montée en premier (correct)
3. ✓ Les GPIO sont-ils bien connectés aux bonnes résistances ?
   → Vérifier câblage physique SSR ↔ broche ESP32

### Symptôme : Distribution bizarre ou asymétrique

**Exemple :** Phase 2 s'ouvre avant Phase 1 complètement fermée

**Cause probable :** Puissances nominales mal renseignées ou GPIO décalés

**Solution :**
1. Aller page Actions
2. Éditer chaque relais géré et cocher le champ Puissance
3. Configurer correctement : **même valeur** si charges identiques (ex. : 1000 W)
4. Sauvegarder et redémarrer

### Diagnostic avancé : Log Telnet

Pour voir la distribution en temps réel :

1. Ouvrir terminal Telnet :
   ```bash
   telnet <IP_ROUTEUR> 23
   ```
   (remplacer `<IP_ROUTEUR>` par l'IP affichée en page Accueil)

2. Une fois connecté, envoyer la commande pour activer le log d'une action :
   ```
   > RetardVx 1
   ```
   (active le log pour le relais géré action 1)

3. Injecter une surproduction (attendre quelques secondes)

4. Regarder le Telnet afficher :
   ```
   [relais 1 <- seq 0]  GroupeOuv= 50.00%  SeuilDemarrage= 0.00%  FractionGroupe= 33.33%  RelaisOuv= 100.00%  Retard= 0
   ```
   → Cela affiche les détails du calcul de distribution

5. Pour arrêter le log :
   ```
   > RetardVx -1
   ```

### Symptôme : Changements de config ne prennent effet qu'après redémarrage

**Comportement normal :** Les configurations pour le séquenceur sont chargées au démarrage.
Toute modification nécessite une sauvegarde Web + redémarrage du routeur.

**Conseil :** Vous pouvez aussi éditer directement `parametres.json` si vous avez accès aux fichiers.
