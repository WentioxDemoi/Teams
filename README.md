# Teams

<p align="center">
  <strong>Application de communication temps réel</strong>
</p>

Teams est une application de communication développée en **C++**, permettant à plusieurs utilisateurs de communiquer à travers une architecture client/serveur.

Le projet est composé de deux applications indépendantes :

- 🖥️ **Teams Client** — application desktop avec interface graphique
- ⚙️ **Teams Server** — backend chargé de gérer les utilisateurs, les communications et les données

L'objectif du projet est de concevoir une application complète en mettant en pratique des concepts d'architecture logicielle, de programmation réseau, de communication asynchrone, de sécurité et de tests automatisés.

---

## 📐 Architecture

Le projet suit une architecture **client/serveur**.

```text
                    Teams Client
                         │
                         │
                  TCP / SSL / JSON
                         │
                         ▼
                    Teams Server
                         │
                         │
                    PostgreSQL
```

Le client fournit l'interface utilisateur et communique avec le serveur.

Le serveur centralise la logique métier, l'authentification, la gestion des sessions, les communications entre utilisateurs ainsi que l'accès aux données.

Cette séparation permet notamment de maintenir une distinction claire entre :

- l'interface utilisateur ;
- la logique de présentation ;
- la logique métier ;
- les communications réseau ;
- la persistance des données.

---

## 🖥️ Teams Client

Le client est une application desktop développée en **C++ avec Qt 6**.

Il utilise une architecture **MVVM (Model-View-ViewModel)** afin de séparer l'interface graphique de la logique de présentation et des services applicatifs.

Technologies principales :

- C++17
- Qt 6.9.3
- QML / Qt Quick
- Qt Network
- Qt Keychain
- CMake
- GoogleTest

### Documentation

Pour l'installation, la compilation, les tests, la création du `.dmg` et les détails de l'architecture du client :

➡️ **[Consulter le README du Teams Client](TeamsClient_/README.md)**

---

## ⚙️ Teams Server

Le serveur est développé en **C++20** et utilise une architecture en couches avec injection de dépendances.

Il est notamment responsable de :

- l'authentification ;
- la gestion des utilisateurs ;
- la gestion des sessions ;
- la messagerie ;
- la gestion des contacts ;
- les communications temps réel ;
- les communications sécurisées SSL/TLS ;
- l'accès à PostgreSQL.

Technologies principales :

- C++20
- Boost.Asio
- PostgreSQL / libpqxx
- OpenSSL
- Argon2
- GoogleTest / GoogleMock
- CMake
- Docker

### Documentation

Pour l'installation, Docker, le DevContainer, la compilation, les tests et la génération du coverage :

➡️ **[Consulter le README du Teams Server](TeamsServer_/README.md)**

---

## 📁 Organisation du projet

Le repository est organisé en deux applications distinctes : le client desktop et le serveur backend.

```text
Teams/
│
├── TeamsClient_/
│   ├── src/
│   │   ├── Core/             # Classes et logique de base
│   │   ├── Database/         # Gestion de la base de données locale
│   │   ├── Models/           # Modèles de données
│   │   ├── QML/              # Interfaces et composants QML
│   │   ├── Repositories/     # Accès et gestion des données
│   │   ├── Services/         # Services applicatifs et réseau
│   │   ├── Utils/            # Utilitaires
│   │   ├── ViewModels/       # Logique de présentation (MVVM)
│   │   ├── includes.h
│   │   ├── main.cpp
│   │   └── mainwindow.ui
│   │
│   ├── Tests/
│   │   ├── ModelTests/       # Tests des modèles
│   │   ├── ServiceTests/     # Tests des services
│   │   ├── ViewModelTests/   # Tests des ViewModels
│   │   └── test_main.cpp
│   │
│   ├── resources/
│   │   ├── icons/            # Icônes
│   │   ├── images/           # Images
│   │   ├── qss/              # Feuilles de style Qt
│   │   └── resources.qrc
│   │
│   ├── third_party/
│   │   ├── libyuv/           # Dépendance libyuv
│   │   ├── webrtc/           # Dépendance WebRTC
│   │   ├── build_webrtc.sh
│   │   └── build_libyuv.sh
│   │
│   ├── CMakeLists.txt
│   ├── Info.plist
│   └── README.md
│
├── TeamsServer_/
│   ├── src/
│   │   ├── Core/
│   │   ├── Handlers/
│   │   ├── Infrastructure/
│   │   ├── Network/
│   │   ├── Utils/
│   │   └── main.cpp
│   │
│   ├── Tests/
│   ├── CMakeLists.txt
│   ├── Dockerfile
│   └── README.md
│
├── docker-compose.yml
├── .env
└── README.md
```

### Teams Client

Le client contient notamment :

- `src/` : code source de l'application ;
- `Tests/` : tests unitaires organisés par type de composant ;
- `resources/` : ressources graphiques et fichiers QML/Qt ;
- `third_party/` : dépendances externes nécessaires notamment à la communication WebRTC ;
- `CMakeLists.txt` : configuration de compilation ;
- `Info.plist` : configuration du bundle macOS.

### Teams Server

Le serveur est organisé selon une architecture en couches :

- `Core/` : logique métier ;
- `Handlers/` : traitement des requêtes ;
- `Infrastructure/` : infrastructure et accès aux données ;
- `Network/` : communication réseau ;
- `Utils/` : fonctionnalités utilitaires ;
- `Tests/` : tests unitaires.

Les détails de l'architecture et de la compilation sont disponibles dans les README propres à chaque application.

---

## 🚀 Démarrage rapide

### Serveur

Le serveur peut être lancé avec Docker Compose :

```bash
docker-compose --env-file .env up --build
```

Le serveur et sa base de données PostgreSQL sont alors démarrés dans leur environnement Docker respectif.

Pour le développement, l'utilisation du **DevContainer** est recommandée.

👉 Les instructions détaillées sont disponibles dans le [README du serveur](TeamsServer_/README.md).

### Client

Le client est compilé avec CMake :

```bash
cd TeamsClient_/build
cmake ..
make
```

Puis l'application peut être lancée depuis le bundle `.app`.

👉 Les instructions détaillées sont disponibles dans le [README du client](TeamsClient_/README.md).

---

## 🧪 Tests

Les deux applications disposent de tests automatisés basés sur **GoogleTest**.

Les procédures de compilation et d'exécution des tests sont documentées dans les README respectifs.

Le serveur dispose également d'un système de **code coverage** permettant de générer un rapport HTML avec `gcovr`.

---

## 🔐 Sécurité

La communication entre le client et le serveur est sécurisée grâce à **SSL/TLS**.

Les mots de passe utilisateurs sont protégés à l'aide de **Argon2**, tandis que l'authentification et la gestion des sessions reposent sur un système de tokens.

> Le projet étant réalisé dans un contexte pédagogique, certains fichiers de configuration et certificats sont présents dans le repository.

---

## 🛠️ Technologies

| Domaine | Technologies |
|---|---|
| Langage | C++17 / C++20 |
| Client | Qt 6, QML, MVVM |
| Serveur | Boost.Asio, architecture en couches |
| Base de données | PostgreSQL |
| Sécurité | OpenSSL, Argon2 |
| Tests | GoogleTest, GoogleMock |
| Build | CMake |
| Environnement | Docker, Docker Compose, DevContainer |
| Coverage | gcovr |

---

## 📊 Diagrammes

Les diagrammes UML et documents d'architecture du projet sont disponibles dans la branche :

```text
diagram
```

---

## 📚 Documentation

| Documentation | Description |
|---|---|
| [Teams Client](TeamsClient_/README.md) | Installation, compilation, Qt, tests et packaging |
| [Teams Server](TeamsServer_/README.md) | Docker, architecture serveur, tests et coverage |
| [Diagrammes](../../tree/diagram) | UML et documentation d'architecture |

---

## 🎯 Objectif du projet

Ce projet a pour objectif de mettre en pratique la conception et le développement d'une application distribuée complète, en combinant :

- développement C++ ;
- architecture logicielle ;
- programmation réseau ;
- communications asynchrones ;
- développement d'une interface graphique ;
- gestion de bases de données ;
- sécurité des communications ;
- injection de dépendances ;
- tests unitaires et mocks ;
- conteneurisation avec Docker.

Le projet est développé progressivement et de nouvelles fonctionnalités sont ajoutées au fur et à mesure de son avancement.

---

## 👥 Projet

**Teams** est un projet réalisé dans un cadre pédagogique.