# Teams Server

Serveur backend pour l'application Teams, gérant l'authentification, la messagerie et la communication temps réel via NATS.

## Architecture du projet

```
src/
├── Database/        # Gestion de la base de données SQLite
├── Servers/         # Serveurs TCP/WebSocket
├── Services/        # Logique métier
├── Sessions/        # Gestion des sessions client
├── Errors/          # Gestion centralisée des erreurs
└── Structs/         # Structures de données
```

## Prérequis

- Docker >= 28.4
- CMake >= 3.16
- Boost >= 1.89.0
- SQLite >= 3.50.4
- GoogleTest >= 1.17.0
- Nats-server >= 2.12.2

## Déploiement avec Docker

Le serveur est compilé et exécuté via Docker, garantissant une cohérence d'environnement avec le déploiement sur EC2.

**Note :** L'installation manuelle de Boost 1.89.0 est complexe et ne fournit pas les fichiers `.cmake` requis par défaut. Docker simplifie ce processus.

### Méthode 1 : Docker Compose (Recommandé)

1. Lancez le serveur et la base de données :
```bash
docker-compose --env-file .env up --build
```

2. Pour réinitialiser l'environnement :
```bash
docker-compose down -v
```

### Méthode 2 : Docker standalone

1. Construisez l'image Docker :
```bash
docker build -t teams-server .
```

2. Lancez le conteneur :
```bash
docker run -p 8080:8080 -p 8081:8081 teams-server
```

## Compilation locale (Développement)

1. Créez un répertoire de build :
```bash
mkdir -p build
cd build
```

2. Configurez le projet avec CMake :
```bash
cmake ..
```

3. Compilez le serveur :
```bash
make
```

4. Lancez le serveur :
```bash
./bin/server
```

## Tests unitaires

Les tests unitaires utilisent **GoogleTest**, intégré automatiquement par CMake via FetchContent.

### Compilation des tests

1. Depuis le répertoire de build :
```bash
cd build
```

2. Compilez les tests :
```bash
make server_tests
```

### Exécution des tests

1. Exécutez le binaire de test :
```bash
./server_tests
```

2. (Optionnel) Sortie détaillée :
```bash
./server_tests --gtest_verbose=1
```

3. (Optionnel) Exécuter un test spécifique :
```bash
./server_tests --gtest_filter=NomDuTest.*
```

4. (Optionnel) Générer un rapport XML (CI/CD) :
```bash
./server_tests --gtest_output=xml:resultats_tests.xml
```

## Configuration

Le serveur utilise un fichier `.env` pour la configuration. Les certificats SSL (`server.crt`, `server.key`) sont fournis dans le dépôt.

**Note :** Ce projet ayant un but pédagogique et ne contenant pas de données sensibles, les fichiers `.env`, certificats SSL et configurations VSCode sont inclus dans le dépôt.

## Ports utilisés

- **8080** : Serveur d'authentification
- **8081** : Serveur de messagerie
- **4222** : NATS (communication inter-services)

## Dépendances

- **Boost.Asio** : Serveurs TCP/WebSocket asynchrones
- **Boost.Beast** : Support WebSocket
- **SQLite3** : Base de données embarquée
- **NATS C Client** : Messagerie pub/sub
- **OpenSSL** : Support TLS/SSL
- **GoogleTest** : Framework de tests unitaires

## Architecture technique

- **Connection Pool** : Gestion optimisée des connexions SQLite
- **Session Management** : Gestion des sessions client avec authentification JWT
- **NATS Integration** : Communication temps réel via publish/subscribe
- **Async I/O** : Traitement asynchrone avec Boost.Asio pour la scalabilité

## Notes importantes

- Le CMake sera enrichi au fur et à mesure du développement
- D'autres fonctionnalités seront ajoutées progressivement
- La base de données SQLite est automatiquement initialisée au premier lancement