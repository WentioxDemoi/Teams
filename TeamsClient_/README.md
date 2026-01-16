# Qt Chat Client - MVVM Architecture

## Structure du projet

```
src/
├── Models/          # Modèles de données
├── ViewModels/      # Logique de présentation
├── Views/           # Interfaces utilisateur
├── Services/        # Services réseau et métier
├── Components/      # Composants réutilisables
├── Utils/           # Utilitaires
└── Core/            # Classes de base
```

## Build

1. Accédez au dossier de build :
```bash
cd build/TeamsClient_
```

2. Générez les fichiers de build avec CMake (en remplaçant le chemin Qt6 selon votre configuration) :
```bash
cmake ../ #Ou pour inclure les tests: cmake ../ -DBUILD_TESTS=ON
```

3. Compilez le projet :
```bash
make
```

4. Lancez l'application :
```bash
./TeamsClient_.app/Contents/MacOS/TeamsClient_
```

Alternativement, vous pouvez double-cliquer sur l'app dans le dossier `build/`.

### Création d'un fichier .dmg

1. Exécutez macdeployqt pour préparer l'application :
```bash
~/Qt/6.9.3/macos/bin/macdeployqt TeamsClient_.app/
```

2. Créez le fichier .dmg :
```bash
hdiutil create -volname "TeamsClient_" -srcfolder TeamsClient_.app -ov -format UDZO TeamsClient_.dmg
```

### Exécution des tests

1. Exécutez le binaire de test :
```bash
ctest -V # Ou alors ./TeamsClient_Tests
```

## Dépendances

- Qt 6.x (Core, Widgets, Network, Test)
- Qt keychain 0.15.0 (installé manuellement dans le repo QT)
    - https://github.com/frankosterfeld/qtkeychain/tree/0.15
    - mkdir build && cd build
    - cmake .. -DCMAKE_PREFIX_PATH=$HOME/Qt/6.9.3/macos -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/Qt/6.9.3/macos -DBUILD_WITH_QT6=ON
    - make -j$(sysctl -n hw.ncpu)
    - make install
- CMake 3.16+
- Compilateur C++17

## Architecture MVVM

- **Model**: Données pures sans logique métier
- **View**: Interface utilisateur (Qt Widgets/QML)
- **ViewModel**: Logique de présentation et binding
- **Services**: Logique métier et communication réseau

## TODO
- Service de check du token enregistré localement en faisant une requete sur le serveur
- Créer db et services associés 
- Faire tests unitaires
