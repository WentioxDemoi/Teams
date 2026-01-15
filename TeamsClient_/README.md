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
cmake ../ -DCMAKE_PREFIX_PATH=$HOME/Qt/6.9.3/macos/lib/cmake/Qt6
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

0. Compilez les tests :
```bash
make TeamsClient_Tests
```

1. Exécutez le binaire de test :
```bash
./TeamsClient_Tests
```

2. (Optionnel) Pour obtenir une sortie plus détaillée :
```bash
./TeamsClient_Tests --gtest_verbose=1
```

3. (Optionnel) Pour exécuter un test spécifique :
```bash
./TeamsClient_Tests --gtest_filter=NomDuTest.*
```

4. (Optionnel) Pour générer un rapport XML (intégration CI/CD) :
```bash
./TeamsClient_Tests --gtest_output=xml:resultats_tests.xml
```


## Dépendances

- Qt 6.x (Core, Widgets, Network, WebSockets)
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
