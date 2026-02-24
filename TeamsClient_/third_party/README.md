# WebRTC (C++ Static Build)

Ce dossier contient une **version compilée de WebRTC** destinée à être utilisée dans un projet C++ avec **CMake**.  

> ⚠️ WebRTC **n'est pas inclus dans ce repo** : il est téléchargé et compilé via le script fourni pour garantir une build **reproductible et isolée**.

---

## Prérequis (macOS)

Assurez-vous d’avoir installé les outils suivants :

```bash
brew install git python ninja
```

Optionnel mais recommandé : clang (déjà inclus sur macOS via Xcode Command Line Tools).

⸻

## Build de WebRTC

Depuis le dossier third_party/webrtc :

```bash
sudo ./build_webrtc.sh
```
**Attention, le script est à modifier en fonction de la platforme cible et le processeur**

Le script fera les étapes suivantes :
	1.	Télécharger depot_tools si nécessaire
	2.	Cloner WebRTC dans src/
	3.	Compiler WebRTC en lib statique (libwebrtc.a)
	4.	Copier les headers nécessaires dans include/
	5.	Copier la lib statique dans lib/

Les headers et la lib sont prêts à être utilisés dans votre projet C++.



## Utilisation dans CMake

Exemple minimal pour inclure WebRTC dans votre cible :

```bash
target_include_directories(my_target PRIVATE
    third_party/webrtc/include
)

target_link_libraries(my_target PRIVATE
    third_party/webrtc/lib/libwebrtc.a
    "-framework CoreFoundation"
    "-framework CoreAudio"
    "-framework AudioToolbox"
    "-framework VideoToolbox"
)
```

## Structure du dossier

third_party/webrtc/
├── build_webrtc.sh        # script de build
├── include/               # headers nécessaires
│   ├── api/
│   └── rtc_base/
├── lib/                   # libwebrtc.a
├── src/                   # code source WebRTC cloné (ne pas push)
└── depot_tools/           # outils Chromium (ne pas push)


## Notes importantes
	•	Ne jamais pusher src/, lib/ ou depot_tools/ dans GitHub.
	•	La build est macOS spécifique ; pour Linux/Windows, adapter le script GN.
	•	WebRTC utilise STUN/TURN et signalisation pour le P2P : le code inclus ici ne fournit pas de serveur de signalisation.
	


## Licence

WebRTC est distribué sous BSD 3-clause. Les licences tierces sont incluses dans le dépôt officiel WebRTC.