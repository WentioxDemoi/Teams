# WebRTC & libyuv (C++ Static Build)

Ce dossier contient des **versions compilées localement et statiquement de WebRTC et libyuv**, destinées à être utilisées dans un projet C++ avec **CMake**.

> ⚠️ Ni WebRTC ni libyuv ne sont inclus dans ce repository : ils sont **téléchargés et compilés via des scripts** afin de garantir une build **reproductible, isolée et maîtrisée**.

---

## Prérequis (macOS)

Assurez-vous d’avoir installé les outils suivants :

```bash
brew install git python ninja cmake
```

---

## Dépendance : WebRTC

WebRTC est une bibliothèque de communication temps réel fournissant :
- audio / vidéo temps réel
- transport réseau sécurisé
- NAT traversal (STUN / TURN)
- primitives bas niveau pour le P2P

Ce repository ne fournit **pas** de serveur de signalisation.

---

## Build de WebRTC

Depuis le dossier `third_party` :

```bash
./build_webrtc.sh
```

⚠️ **Le script doit être adapté à la plateforme cible et à l’architecture CPU.**

Le script effectue les étapes suivantes :
1. Télécharge `depot_tools` si nécessaire
2. Clone WebRTC dans `src/`
3. Compile WebRTC en bibliothèque statique (`libwebrtc.a`)
4. Copie les headers nécessaires dans `include/`
5. Copie la librairie statique dans `lib/`

Les headers et la librairie sont alors prêts à être utilisés dans un projet C++.

---

## Dépendance : libyuv

libyuv est une bibliothèque bas niveau utilisée par WebRTC pour :
- conversions de formats YUV / RGB
- scaling et rotation d’images
- optimisations SIMD (ARM / x86)

Elle est construite **localement et indépendamment**, afin d’éviter toute dépendance système implicite.

Selon le projet, libyuv peut être :
- liée directement par l’application
- ou utilisée indirectement via WebRTC

---

## Build de libyuv

Depuis le dossier `third_party` :

```bash
./build_libyuv.sh
```

Le script effectue les étapes suivantes :
1. Clone libyuv depuis le dépôt Chromium officiel
2. Configure une build Release pour macOS arm64
3. Compile la librairie
4. Installe les headers et la librairie statique localement

⚠️ Le script est actuellement configuré pour **macOS arm64**.  
Pour `x86_64` ou d’autres plateformes, adapter `CMAKE_OSX_ARCHITECTURES`.

---

## Utilisation dans CMake

### WebRTC

Aller voir dans le cmake actuel

### libyuv

Aller voir dans le cmake actuel
```

> Si libyuv est déjà intégrée statiquement dans WebRTC, il n’est pas nécessaire de la lier séparément.

---

## Structure du dossier

```
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

- Ne jamais pusher dans Git :
  - `webrtc/src/`
  - `webrtc/lib/`
  - `webrtc/depot_tools/`
  - `libyuv/libyuv_src/`
  - `libyuv/libyuv/`
- Les builds sont **spécifiques à macOS**
- Pour Linux ou Windows, les scripts GN / CMake doivent être adaptés
- Aucune infrastructure réseau (signalisation, TURN) n’est fournie ici

---

## Plateformes supportées

- macOS
  - arm64 ✅
  - x86_64 ⚠️ (script à adapter)
- Linux ❌
- Windows ❌

---

## Licence

- WebRTC est distribué sous licence BSD 3-clause
- libyuv est distribué sous licence BSD 3-clause
- Les licences tierces sont incluses dans les dépôts officiels respectifs
