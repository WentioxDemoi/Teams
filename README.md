# Teams

Le but est de recoder une application similaire à Teams.

## Prérequis

- CMake >= 3.16
- Boost >= 1.89.0
- Qt6 (Idéalement Qt 6.9.3)
- Docker >= 28.4
- SQLite >= 3.50.4

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

### Étapes de compilation et d'exécution

1. Construisez l'image Docker :
```bash
docker build -t server .
```

2. Lancez le serveur :
```bash
docker run -it server
```

## Notes importantes

- Le CMake sera amélioré pour intégrer plus d'outils au fur et à mesure du développement.
- D'autres dépendances seront ajoutées à l'avenir.
- Assurez-vous de bien spécifier le chemin vers Qt6 lors de la compilation du client.