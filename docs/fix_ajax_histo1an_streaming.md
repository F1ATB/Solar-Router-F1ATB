# Fix : `ajax_histo1an` — passage en streaming natif WebServer

**Version** : V17.17+ | **Date** : Mars 2026 | **Auteur** : F1ATB

## Contexte

L'endpoint `/ajax_histo1an` renvoie l'historique d'énergie journalière des 3 derniers mois,
lu depuis des fichiers CSV mensuels stockés sur LittleFS.

Il était appelé depuis le frontend (`LoadHisto1an` dans `JS_Accueil.h`) au chargement de la
page d'accueil, immédiatement avant le chargement de l'historique 48h.

En production, cette route retournait un **502** de façon intermittente.

---

## Cause du problème

L'ancienne implémentation (`envoyerHistoriqueEnergie`) procédait en deux étapes :

1. **Accumulation** : toutes les lignes CSV valides étaient ajoutées dans un `JsonDocument`
   ArduinoJson en mémoire RAM.
2. **Envoi chunked artisanal** : la sérialisation JSON était effectuée via une classe `ChunkedWriter`
   custom qui écrivait elle-même le framing HTTP chunked (`<taille HEX>\r\n<données>\r\n`) sur
   le socket TCP brut, puis concluait par `0\r\n\r\n`.

Ce schéma présentait deux fragilités combinées qui pouvaient produire un 502 :

- **Interaction non définie avec `WebServer`** : `WebServer` (bibliothèque ESP32 Arduino) peut
  avoir déjà émis des en-têtes ou modifié l'état du socket. Écrire directement sur le client
  TCP sous-jacent après un `send()` classique peut produire une réponse HTTP mal formée ou
  incomplète, que le navigateur interprète comme un 502.
- **Pression mémoire** : la construction du `JsonDocument` avant envoi consommait de la RAM
  de façon prévisible pour un historique de ~270 lignes (~8 Ko), sans bénéfice fonctionnel.

---

## Solution retenue

Remplacement par un **streaming JSON direct** via l'API `sendContent` du `WebServer`,
sans accumulation préalable.

### Fonctionnement de `sendContent` et implications réseau/TCP

#### Mécanisme HTTP : Transfer-Encoding chunked

Lorsqu'on appelle `setContentLength(CONTENT_LENGTH_UNKNOWN)` avant `send()`, la bibliothèque
`WebServer` d'Arduino ESP32 ajoute automatiquement l'en-tête HTTP :

```
Transfer-Encoding: chunked
```

Cela signifie que la réponse n'a pas de longueur annoncée à l'avance. Le client HTTP sait
qu'il doit lire jusqu'à réception d'un chunk terminal vide.

Le format HTTP chunked est :
```
<longueur en hexadécimal>\r\n
<données>\r\n
...
0\r\n
\r\n      ← chunk terminal vide : fin de la réponse
```

**Important** : `WebServer` gère intégralement ce framing. Chaque appel à `sendContent(data)`
est traduit en interne par un chunk correctement formé. Le chunk terminal `0\r\n\r\n` est
émis automatiquement à la fin du handler, quand `WebServer` finalize la connexion.

C'est précisément ce que l'ancienne `ChunkedWriter` faisait manuellement — et de façon
risquée, car elle écrivait directement sur le socket TCP sous-jacent alors que `WebServer`
avait potentiellement déjà envoyé des octets sur ce socket.

#### Ce qui se passe côté TCP (pile lwIP de l'ESP32)

L'ESP32 utilise la pile TCP **lwIP**. Plusieurs mécanismes influencent la segmentation réelle
des données sur le réseau :

**1. Buffer d'émission TCP (send buffer)**

lwIP dispose d'un buffer d'envoi par socket (typiquement 5 à 10 Ko sur ESP32). Les appels
successifs à `sendContent()` ne génèrent pas forcément un paquet TCP par appel. lwIP accumule
les données dans ce buffer et décide de l'envoi selon :
- le remplissage du buffer (flush automatique quand il est plein)
- l'algorithme de Nagle (voir ci-dessous)
- un appel explicite à `flush()` ou la fermeture de la connexion

**2. Algorithme de Nagle**

Par défaut actif sur lwIP, Nagle retarde l'envoi de petits paquets si des données sont déjà
en transit non acquittées. Concrètement, dans notre cas :

```
sendContent("{\"EnergieJour\":[")   →  ~18 octets, probablement bufferisé
sendContent(",")                    →  1 octet, bufferisé par Nagle
sendContent("\"")                   →  1 octet, bufferisé
sendContent(jsonEscape(ligne))      →  ~30 octets, bufferisé
sendContent("\"")                   →  1 octet, bufferisé
... (prochain tour de boucle)
sendContent("]}")                   →  2 octets
```

En pratique, lwIP regroupe ces fragments en **2 à 5 segments TCP** de ~1460 octets (MSS
Ethernet standard), indépendamment du découpage logique de `sendContent`. L'overhead du
framing chunked (environ 6 octets par chunk : taille hex + `\r\n` x2) est donc marginal
sur le trafic total.

**3. Trafic réseau observé pour une réponse typique**

Pour un historique de ~270 lignes sur 3 mois (~9 Ko de données JSON brutes) :

| Phase | Taille approximative |
|---|---|
| En-têtes HTTP de réponse | ~150 octets |
| Framing chunked (overhead) | ~1,6 Ko (6 octets × 270 chunks) |
| Données JSON utiles | ~9 Ko |
| Chunk terminal | 5 octets |
| **Total TCP émis** | **~11 Ko** |

Échangé en **8 à 10 segments TCP** sur WiFi 2.4 GHz, typiquement en moins de 50 ms sur
réseau local. Négligeable.

**4. Fermeture de connexion**

Après le retour du handler, `WebServer` appelle `client.stop()`. lwIP envoie alors le chunk
terminal `0\r\n\r\n`, suivi d'un FIN TCP. Le navigateur reçoit le signal de fin de réponse
et peut terminer le `JSON.parse()`.

> C'est ici que l'ancienne `ChunkedWriter` était fragile : si elle émettait le `0\r\n\r\n`
> *avant* que `WebServer` referme la connexion, ou si `WebServer` émettait ses propres
> octets de clôture après, le client pouvait recevoir une séquence TCP invalide.

### Principe

```
setContentLength(CONTENT_LENGTH_UNKNOWN)
send(200, "application/json", "")      ← ouvre la réponse

sendContent("{\"EnergieJour\":[")       ← début du JSON

pour chaque fichier mensuel valide :
    lire ligne par ligne
    pour chaque ligne utile :
        sendContent(",")               ← séparateur (sauf première)
        sendContent("\"<ligne>\"")     ← valeur JSON

sendContent("]}")                      ← fin du JSON
```

### Fichiers modifiés

| Fichier | Changement |
|---|---|
| `Server.ino` | Réécriture de `envoyerHistoriqueEnergie()`, suppression de la classe `ChunkedWriter` |
| `JS_Accueil.h` | Aucun — le contrat de données est inchangé |

### Impact RAM

| Avant | Après |
|---|---|
| JsonDocument alloué (~270 entrées) | Une seule `String ligne` à la fois (≤ 64 octets) |

---

## Contrat de données — inchangé

Le frontend (`LoadHisto1an`) appelle `JSON.parse()` sur la réponse texte et accède à
`retour.EnergieJour` comme tableau de chaînes CSV. Ce contrat est strictement préservé.

Format de réponse :
```json
{
  "EnergieJour": [
    "2026-01-01,1234,567,1800,2100",
    "2026-01-01,2234,667,1900,2200",
    ...
  ]
}
```

Chaque élément est une ligne CSV brute issue des fichiers `Mois_Wh_YYYYMM.csv`.
Le frontend déduplique lui-même les entrées multiples d'une même journée (plusieurs mesures
par jour) en ne conservant que la dernière valeur vue par date.

---

## Détails d'implémentation

### Fenêtre de données

Les 3 fichiers mensuels couvrant `M-2`, `M-1` et `M` (mois courant) sont lus séquentiellement.
Si un fichier n'existe pas, il est silencieusement ignoré. Si un fichier existe mais ne peut
pas être ouvert (`LittleFS.open` retourne un handle invalide), la boucle passe au suivant.

### Filtrage des lignes

Une ligne est incluse si :
- `longueur > 10` (élimine les lignes vides ou trop courtes)
- ne contient pas `"Date,"` (élimine la ligne d'en-tête CSV)

### Échappement JSON

Une fonction locale `jsonEscape(String)` échappe les caractères `"` et `\` dans chaque ligne
avant émission. Les autres caractères de contrôle (`\r`, `\n`, `\t`) ne sont **pas** traités.
Ceci est acceptable dans la mesure où les fichiers CSV sont produits en interne par le firmware
lui-même, dans un format contrôlé.

> ⚠️ Si le format des fichiers CSV évolue pour inclure des champs texte libres, l'échappement
> devra être complété.

---

## Limites connues

### 1. Pas de rollback en cas d'erreur en cours d'envoi
Une fois `send(200, ...)` appelé, la réponse HTTP est engagée. Une erreur LittleFS survenant
après le début de l'envoi produira un JSON tronqué côté client, sans code d'erreur HTTP
utilisable. Le frontend (`LoadHisto1an`) catchera l'exception `JSON.parse` et logguera
`"Erreur LoadHisto1an"` dans la console, mais le graphique annuel ne sera simplement pas
affiché — comportement acceptable.

### 2. Échappement JSON minimal
Voir section ci-dessus.

### 3. Pas de compression
La réponse est envoyée en clair. Pour une fenêtre de 3 mois, la taille est d'environ 8 à
12 Ko, ce qui est négligeable sur WiFi local. Si la fenêtre devait être élargie, une
compression gzip nécessiterait un refactor complet (préchargement ou streaming zlib).
