# Teams Server

Serveur backend pour l'application Teams, gérant l'authentification, la messagerie et la communication temps réel.

## Architecture du projet

```text
src/
├── Core/            # Logique métier principale et modèles
│   ├── Models/      # Structures de données (ex: User)
│   ├── Registeries/ # Pool de sessions utilisateurs
│   ├── Repositories/# Accès aux données (UserRepository, etc.)
│   └── Services/    # Logique métier (AuthService, etc.)
├── Handlers/        # Gestion des requêtes entrantes et routage
├── Infrastructure/  # Gestion technique (DB, ConnectionPool, QueryBuilder, etc.)
├── Network/         # Serveurs TCP/SSL et sessions réseau (TcpListener, AuthSession)
├── Utils/           # Utilitaires généraux (Crypto, Config, ResponseFormater, BoostErrorHandler)
└── main.cpp         # Point d'entrée de l'application

C'est une architecture en layer avec injection de dépendances
```

## Prérequis

- Docker >= 28.4.0
- CMake >= 3.22.1
- Boost >= 1.74.0
- SQLite >= 3.54.0
- GoogleTest >= 1.17.0
- g++ >= 11.4.0

## Déploiement avec Docker

Le serveur est compilé et exécuté via Docker, garantissant une cohérence d'environnement avec le déploiement sur EC2.

### Méthode 1 : Docker Compose

1. Lancez le serveur et la base de données :
```bash
docker-compose --env-file .env up --build
```

2. Pour réinitialiser l'environnement :
```bash
docker-compose down -v
```

### Méthode 2 : DevContainer (Recommandé)

1. Lancez le dev container via VSCode (CMD + SHIFT + P) --> Reconstruire le container

2. Une fois dedans, compilez et lancez manuellement.

## Tests unitaires

Les tests unitaires utilisent **GoogleTest**, intégré automatiquement par CMake via FetchContent.

### Compilation et exécution des tests

1. Dans le CMake, décommentez la partie dédiée aux tests.
2. Dans le Dockerfile, commentez la ligne qui lance le serveur et décommentez celle qui lance le terminal.
3. Une fois connecté au terminal du DevContainer, compilez le projet puis lancez les tests :

```bash
cd build/
./server_tests
```

### Génération du coverage

Le projet utilise **gcovr** afin de générer un rapport HTML permettant de visualiser graphiquement la couverture du code.

Après avoir exécuté les tests, toujours depuis le dossier `build/`, lancez :

```bash
gcovr \
    --root .. \
    --filter '../src/' \
    --html-details coverage.html
```

Cette commande génère le fichier :

```text
build/coverage.html
```

Ouvrez ensuite `coverage.html` dans un navigateur pour consulter le rapport de couverture, notamment la couverture par fichier et les lignes de code couvertes ou non couvertes.
 
> **Remarque :** les tests doivent être exécutés avant la génération du rapport afin que les fichiers de données de couverture soient correctement générés.

## Configuration

Le serveur utilise un fichier `.env` pour la configuration. Les certificats SSL (`server.crt`, `server.key`) sont fournis dans le dépôt.

**Note :** Ce projet ayant un but pédagogique et ne contenant pas de données sensibles, les fichiers `.env`, certificats SSL et configurations VSCode sont inclus dans le dépôt.

## Ports utilisés

- **8080** : Serveur d'authentification
- **8082** : Serveur de messagerie
- **8083** : Serveur d'échange WebRTC
- **8084** : Serveur des contacts

## Dépendances

- **Boost.Asio** : Serveurs TCP asynchrones et gestion des threads pour le pool de workers
- **libpqxx / PostgreSQL** : Accès à la base de données PostgreSQL
- **OpenSSL** : Support TLS/SSL
- **GoogleTest & GoogleMock** : Tests unitaires et mocks pour AuthService et UserRepository
- **Argon2** : Hachage sécurisé des mots de passe
- **gcovr** : Génération du rapport HTML de couverture du code

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

## TODO

- Refactor le système d'include pour soulager clang
```