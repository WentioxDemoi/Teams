# WebRTC & libyuv (C++ Static Build)

Ce dossier contient des **versions compilées localement et statiquement de WebRTC et libyuv**, destinées à être utilisées dans un projet C++ avec **CMake**.

Les sources complètes de WebRTC et libyuv ne sont pas incluses dans ce repository. Elles sont téléchargées et compilées via des scripts afin de conserver un environnement de build maîtrisé.

> ⚠️ Les versions de WebRTC sont susceptibles d'évoluer. Les commits utilisés pour les builds fonctionnels sont documentés ci-dessous afin de pouvoir retrouver les versions correspondantes si nécessaire.

---

## Prérequis (macOS)

Assurez-vous d'avoir installé les outils suivants :

```bash
brew install git python ninja cmake
```

---

## Dépendance : WebRTC

WebRTC est une bibliothèque de communication temps réel fournissant notamment :

- audio / vidéo temps réel
- transport réseau sécurisé
- NAT traversal (STUN / TURN)
- primitives bas niveau pour le P2P

Ce repository ne fournit **pas** de serveur de signalisation.

### Version WebRTC utilisée

Le build fonctionnel actuel a été réalisé avec le commit suivant :

```text
WebRTC commit:
c4f5b6ef7e5711428a6e433eb7c8d9d3593e7e1a
```

>
> WebRTC est récupéré via `fetch` / `gclient sync`. En cas de problème de compilation ou de changement de comportement, il peut donc être nécessaire de revenir manuellement au commit fonctionnel indiqué ci-dessus.

Si une nouvelle version de WebRTC est utilisée, il faudra potentiellement adapter le script de build, notamment en cas de modification de l'API, des fichiers `BUILD.gn`, des dépendances ou des options GN.

---

## Dépendance : depot_tools

`depot_tools` est l'ensemble d'outils utilisé par Chromium/WebRTC pour récupérer et gérer les sources et dépendances du projet, notamment via `fetch` et `gclient`.

Le script de build utilise une **version explicitement figée de `depot_tools`** afin d'éviter qu'une mise à jour ultérieure des outils ne modifie le comportement du build.

### Version utilisée

```text
depot_tools commit:
6fbb6012d6138844379a2df23eea5e86a2ea6696
```

Le script clone automatiquement `depot_tools` s'il n'est pas déjà présent, puis effectue un checkout sur le commit indiqué.

Il n'est donc normalement **pas nécessaire de sélectionner manuellement la version de `depot_tools`**.

---

## Build de WebRTC

Depuis le dossier `third_party` :

```bash
./build_webrtc.sh
```

Le script effectue les étapes suivantes :

1. Crée le répertoire de travail WebRTC.
2. Télécharge `depot_tools` s'il n'est pas déjà présent.
3. Checkout automatiquement la version figée de `depot_tools`.
4. Configure `gclient`.
5. Télécharge et synchronise les sources et dépendances WebRTC.
6. Applique le patch nécessaire à `BUILD.gn`.
7. Génère la configuration GN.
8. Compile WebRTC en bibliothèque statique (`libwebrtc.a`).
9. Copie la librairie dans `lib/`.

### Compatibilité WebRTC

Le build actuel a été testé avec :

```text
WebRTC:
c4f5b6ef7e5711428a6e433eb7c8d9d3593e7e1a

depot_tools:
6fbb6012d6138844379a2df23eea5e86a2ea6696
```

Si le build ne fonctionne plus après un nouveau `gclient sync`, vérifier en priorité la révision WebRTC utilisée.

Pour revenir à la version fonctionnelle, il peut être nécessaire de checkout manuellement le commit WebRTC indiqué ci-dessus avant de relancer la synchronisation/build.

> ⚠️ Le script est actuellement configuré pour **macOS arm64**.
>
> ⚠️ Les options GN et le patch `BUILD.gn` dépendent de la version de WebRTC. Une nouvelle version de WebRTC peut donc nécessiter des adaptations du script.

---

## Dépendance : libyuv

libyuv est une bibliothèque bas niveau utilisée par WebRTC pour :

- conversions de formats YUV / RGB
- scaling et rotation d'images
- optimisations SIMD (ARM / x86)

Elle est construite **localement et indépendamment**, afin d'éviter toute dépendance système implicite.

Selon le projet, libyuv peut être :

- liée directement par l'application ;
- ou utilisée indirectement via WebRTC.

---

## Build de libyuv

Depuis le dossier `third_party` :

```bash
./build_libyuv.sh
```

Le script effectue les étapes suivantes :

1. Clone libyuv depuis le dépôt Chromium officiel.
2. Configure une build Release pour macOS arm64.
3. Compile la librairie.
4. Installe les headers et la librairie statique localement.

> ⚠️ Le script est actuellement configuré pour **macOS arm64**.
>
> Pour `x86_64` ou d'autres plateformes, adapter `CMAKE_OSX_ARCHITECTURES`.

> ⚠️ Si libyuv est déjà intégrée statiquement dans WebRTC, il n'est pas nécessaire de la lier séparément.

---

## Utilisation dans CMake

### WebRTC

Voir le `CMakeLists.txt` actuel du projet pour l'intégration de WebRTC.

### libyuv

Voir le `CMakeLists.txt` actuel du projet pour l'intégration de libyuv.

---

## Structure du dossier

```text
third_party/
├── build_libyuv.sh
├── build_webrtc.sh
│
├── webrtc/
│   ├── include/
│   ├── lib/
│   ├── src/           # ne pas pusher
│   └── depot_tools/   # ne pas pusher
│
├── libyuv/
│   ├── libyuv/
│   │   ├── include/
│   │   └── lib/
│   └── libyuv_src/    # ne pas pusher
```

---

## Notes importantes

Ne jamais pusher dans Git les sources et outils téléchargés localement :

- `webrtc/src/`
- `webrtc/depot_tools/`
- `libyuv/libyuv_src/`

Les librairies et headers générés localement ne sont également pas destinés à être versionnés dans ce repository, sauf indication contraire.

Les builds sont actuellement **spécifiques à macOS arm64**.

Pour Linux ou Windows, les scripts GN / CMake devront être adaptés.

Aucune infrastructure réseau (signalisation, TURN) n'est fournie ici.

---

## Plateformes supportées

- macOS
  - arm64 ✅
  - x86_64 ⚠️ (script à adapter)
- Linux ❌
- Windows ❌

---

## Reproductibilité et versions fonctionnelles

Les versions fonctionnelles de WebRTC sont conservées comme référence afin d'éviter de devoir retrouver ultérieurement une combinaison de versions compatible.

Le commit `depot_tools` est **figé directement dans le script de build**.

Le commit WebRTC fonctionnel est **documenté dans ce README**, mais n'est pas actuellement forcé automatiquement par le script.

Cette distinction est volontaire : une évolution de WebRTC peut nécessiter une adaptation du patch `BUILD.gn`, des options GN ou du code C++ utilisant WebRTC.

En cas de problème avec une nouvelle version, revenir à la version WebRTC connue comme fonctionnelle permet de retrouver l'environnement de compilation utilisé précédemment.

---

## Licence

- WebRTC est distribué sous licence BSD 3-clause.
- libyuv est distribué sous licence BSD 3-clause.
- Les licences tierces sont incluses dans les dépôts officiels respectifs.