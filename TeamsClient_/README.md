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
cd build/
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

## Preview

1. Accédez au dossier de build :
```bash
cd build/
```

2. Lancer la commande sur le fichier QML cible : 
```bash
~/Qt/6.9.3/macos/bin/qml ../src/QML/Main.qml
```

## File cleaning

1. Accédez au dossier de build :
```bash
cd build/
```

2. Lancer la commande sur le fichier QML cible : 
```bash
~/Qt/6.9.3/macos/bin/qmlformat ../src/QML/Main.qml
```

### Accès à la caméra

- Suppression des permissions sur MacOS : tccutil reset Camera

### Accès au Micro

- Suppression des permissions sur MacOS : tccutil reset Microphone


### Création d'un fichier .dmg

1. Exécutez macdeployqt pour préparer l'application :
```bash
~/Qt/6.9.3/macos/bin/macdeployqt TeamsClient_.app/ -qmldir=/Users/remidesbordes/Documents/Teams/TeamsClient_/src/QML
```

2. Supprimer les plugins SQL non utilisés :
```bash
find TeamsClient_.app -name "libqsqlodbc.dylib" -delete
find TeamsClient_.app -name "libqsqlpsql.dylib" -delete
```

3. Re-signature après toutes les modifications du bundle :
```bash
codesign --force --deep --sign - TeamsClient_.app
```

3. Créez le fichier .dmg :
```bash
hdiutil create -volname "TeamsClient_" -srcfolder TeamsClient_.app -ov -format UDZO TeamsClient_.dmg
```



### Exécution des tests

1. Exécutez le binaire de test :
```bash
ctest -V # Ou alors ./TeamsClient_Tests
```

## Dépendances

- Qt 6.9.3 (Core, Widgets, Network, Test)
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
- Lorsqu'on se connecte au même compte avec deux instance en même temps, on casse la session, il faut redémarrer le software
- problème lors de la fermeture du software après un appel. Le software reste actif et ne se shutdown pas

