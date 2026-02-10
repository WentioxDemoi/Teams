# Teams Server

Serveur backend pour l'application Teams, gérant l'authentification, la messagerie et la communication temps réel.

## Architecture du projet

```
src/
├── Core/            # Logique métier principale et modèles
│   ├── Models/      # Structures de données (ex: User)
│   ├── Repositories/# Accès aux données (UserRepository, etc.)
│   └── Services/    # Logique métier (AuthService, etc.)
├── Handlers/        # Gestion des requêtes entrantes et routage
├── Infrastructure/  # Gestion technique (DB, ConnectionPool, QueryBuilder, etc.)
├── Network/         # Serveurs TCP/SSL et sessions réseau (TcpListener, AuthSession)
├── Utils/           # Utilitaires généraux (Crypto, Config, ResponseFormater, BoostErrorHandler)
├── common/          # Code partagé / commun à plusieurs modules
├── includes.h       # Fichier d'inclusions centralisées
└── main.cpp         # Point d'entrée de l'application
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

1. Dans le CMake, décommentez la partie pour les tests. Dans le dockerfile commentez la ligne qui lance le serveur et décommentez celle qui lance le terminal. Une fois connecté au terminal, faites :
```bash
cd build/ && ./server_tests
```


## Configuration

Le serveur utilise un fichier `.env` pour la configuration. Les certificats SSL (`server.crt`, `server.key`) sont fournis dans le dépôt.

**Note :** Ce projet ayant un but pédagogique et ne contenant pas de données sensibles, les fichiers `.env`, certificats SSL et configurations VSCode sont inclus dans le dépôt.

## Ports utilisés

- **8080** : Serveur d'authentification
- **8081** : Serveur de messagerie
- **4222** : NATS (communication inter-services)

## Dépendances

- **Boost.Asio** : Serveurs TCP asynchrones et gestion des threads pour le pool de workers
- **Boost.Beast** : Support WebSocket (si utilisé dans le futur)
- **libpqxx / PostgreSQL** : Accès à la base de données PostgreSQL
- **NATS C Client** : Messagerie pub/sub
- **OpenSSL** : Support TLS/SSL
- **GoogleTest & GoogleMock** : Tests unitaires et mocks pour AuthService et UserRepository
- **Argon2** : Hachage sécurisé des mots de passe

## Architecture technique

- **Connection Pool** : Gestion optimisée des connexions PostgreSQL via ConnectionPool et DatabaseManager
- **Session Management** : Gestion des sessions client sécurisées via AuthSession avec SSL et authentification par token
- **Handler Layer** : Handler et AuthHandler pour router et traiter les requêtes JSON
- **Async I/O** : Traitement asynchrone des connexions et requêtes via asio::io_context et thread_pool
- **Core Layer** : Logique métier centralisée dans Core/Services et accès aux données via Core/Repositories
- **Utils Layer** : Outils génériques pour la sécurité (Crypto), configuration (Config) et formatage JSON (ResponseFormater)

## Notes importantes

- Le CMake sera enrichi au fur et à mesure du développement
- D'autres fonctionnalités seront ajoutées progressivement
- Des UMLs sont présents dans la branch diagram