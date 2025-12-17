# Teams

Recoding une application similaire à Teams.

## Prérequis

- CMake >= 3.16
- Boost >= 1.89.0
- Qt6 (Idéalement Qt 6.9.3)
- Docker >= 28.4
- SQLite >= 3.50.4
- GoogleTest >= 1.17.0
- Nats-server >= 2.12.2

## Compilation du Client

### Étapes de compilation

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

## Compilation du Serveur

Le serveur est compilé et exécuté via Docker, ce qui garantit une cohérence d'environnement avec le déploiement sur EC2.

**Note :** L'installation manuelle de Boost 1.89.0 est complexe et ne fournit pas les fichiers `.cmake` requis par défaut. Docker simplifie ce processus.

### Étapes de compilation et d'exécution (Ancienne version)

1. Lancer le serveur et la Db via le docker-compose :
```bash
docker-compose --env-file .env up --build
```

2. Si jamais, pour reset :
```bash
docker-compose down -v
```

### Étapes de compilation et d'exécution (Nouvelle version)

1. Construisez l'image Docker :
```bash
docker build -t server .
```

2. Lancez le serveur :
```bash
docker ru

## Tests unitaires (GoogleTest)

Les tests unitaires utilisent **GoogleTest**, intégré automatiquement par CMake.

### Compilation des tests

1. Depuis le répertoire de build :
```bash
cd build/TeamsClient_
```

2. Générez le projet avec les tests :
```bash
cmake ../ -DCMAKE_PREFIX_PATH=$HOME/Qt/6.9.3/macos/lib/cmake/Qt6
```

3. Compilez les tests :
```bash
make TeamsClient_Tests
```

### Exécution des tests

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

## Notes importantes

- Le CMake sera amélioré pour intégrer plus d'outils au fur et à mesure du développement.
- D'autres dépendances seront ajoutées à l'avenir.
- Assurez-vous de bien spécifier le chemin vers Qt6 lors de la compilation du client.
- Étant donné que ce projet a un but pédagogique et ne contient pas de données sensibles, les fichiers .env, .vscode, .crt et .key ont été ajoutés au dépôt.
