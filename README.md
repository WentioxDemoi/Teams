# Teams

<p align="center">
  <strong>Real-Time Communication Application</strong>
</p>

Teams is a communication application developed in **C++**, allowing multiple users to communicate through a client/server architecture.

The project consists of two independent applications:

- 🖥️ **Teams Client** — desktop application with a graphical user interface
- ⚙️ **Teams Server** — backend responsible for managing users, communications, and data

The goal of the project is to design a complete application while applying concepts related to software architecture, network programming, asynchronous communication, security, and automated testing.

---

## 📐 Architecture

The project follows a **client/server architecture**.

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

The client provides the user interface and communicates with the server.

The server centralizes business logic, authentication, session management, communication between users, and data access.

This separation makes it possible to maintain a clear distinction between:

- the user interface;
- presentation logic;
- business logic;
- network communication;
- data persistence.

---

## 🖥️ Teams Client

The client is a desktop application developed in **C++ with Qt 6**.

It uses an **MVVM (Model-View-ViewModel)** architecture to separate the graphical interface from presentation logic and application services.

Main technologies:

- C++17
- Qt 6.9.3
- QML / Qt Quick
- Qt Network
- Qt Keychain
- CMake
- GoogleTest

### Documentation

For installation, compilation, testing, `.dmg` creation, and details about the client architecture:

➡️ **[See the Teams Client README](TeamsClient_/README.md)**

---

## ⚙️ Teams Server

The server is developed in **C++20** and uses a layered architecture with dependency injection.

It is responsible for:

- authentication;
- user management;
- session management;
- messaging;
- contact management;
- real-time communication;
- secure SSL/TLS communication;
- PostgreSQL access.

Main technologies:

- C++20
- Boost.Asio
- PostgreSQL / libpqxx
- OpenSSL
- Argon2
- GoogleTest / GoogleMock
- CMake
- Docker

### Documentation

For installation, Docker, DevContainer, compilation, testing, and coverage generation:

➡️ **[See the Teams Server README](TeamsServer_/README.md)**

---

## 📁 Project Structure

The repository is organized into two separate applications: the desktop client and the backend server.

```text
Teams/
│
├── TeamsClient_/
│   ├── src/
│   │   ├── Core/             # Core classes and logic
│   │   ├── Database/         # Local database management
│   │   ├── Models/           # Data models
│   │   ├── QML/              # QML interfaces and components
│   │   ├── Repositories/     # Data access and management
│   │   ├── Services/         # Application and network services
│   │   ├── Utils/            # Utilities
│   │   ├── ViewModels/       # Presentation logic (MVVM)
│   │   ├── includes.h
│   │   ├── main.cpp
│   │   └── mainwindow.ui
│   │
│   ├── Tests/
│   │   ├── ModelTests/       # Model tests
│   │   ├── ServiceTests/     # Service tests
│   │   ├── ViewModelTests/   # ViewModel tests
│   │   └── test_main.cpp
│   │
│   ├── resources/
│   │   ├── icons/            # Icons
│   │   ├── images/           # Images
│   │   ├── qss/              # Qt style sheets
│   │   └── resources.qrc
│   │
│   ├── third_party/
│   │   ├── libyuv/           # libyuv dependency
│   │   ├── webrtc/           # WebRTC dependency
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

The client contains:

- `src/`: application source code;
- `Tests/`: unit tests organized by component type;
- `resources/`: graphical resources and QML/Qt files;
- `third_party/`: external dependencies required, in particular, for WebRTC communication;
- `CMakeLists.txt`: build configuration;
- `Info.plist`: macOS bundle configuration.

### Teams Server

The server is organized according to a layered architecture:

- `Core/`: business logic;
- `Handlers/`: request processing;
- `Infrastructure/`: infrastructure and data access;
- `Network/`: network communication;
- `Utils/`: utility functionality;
- `Tests/`: unit tests.

Detailed architecture and build instructions are available in the README files for each application.

---

## 🚀 Quick Start

### Server

The server can be started using Docker Compose:

```bash
docker-compose --env-file .env up --build
```

The server and its PostgreSQL database are then started in their respective Docker environments.

For development, using the **DevContainer** is recommended.

👉 Detailed instructions are available in the [server README](TeamsServer_/README.md).

### Client

The client is built using CMake:

```bash
cd TeamsClient_/build
cmake ..
make
```

The application can then be launched from the `.app` bundle.

👉 Detailed instructions are available in the [client README](TeamsClient_/README.md).

---

## 🧪 Tests

Both applications include automated tests based on **GoogleTest**.

Test compilation and execution procedures are documented in their respective README files.

The server also includes a **code coverage** system that can generate an HTML report using `gcovr`.

---

## 🔐 Security

Communication between the client and server is secured using **SSL/TLS**.

User passwords are protected using **Argon2**, while authentication and session management rely on a token-based system.

> Since the project is developed for educational purposes, some configuration files and certificates are included in the repository.

---

## 🛠️ Technologies

| Area | Technologies |
|---|---|
| Language | C++17 / C++20 |
| Client | Qt 6, QML, MVVM |
| Server | Boost.Asio, layered architecture |
| Database | PostgreSQL |
| Security | OpenSSL, Argon2 |
| Testing | GoogleTest, GoogleMock |
| Build | CMake |
| Environment | Docker, Docker Compose, DevContainer |
| Coverage | gcovr |

---

## 📊 Diagrams

The project's UML diagrams and architecture documents are available in the following branch:

```text
diagram
```

---

## 📚 Documentation

| Documentation | Description |
|---|---|
| [Teams Client](TeamsClient_/README.md) | Installation, compilation, Qt, testing, and packaging |
| [Teams Server](TeamsServer_/README.md) | Docker, server architecture, testing, and coverage |
| [Diagrams](../../tree/diagram) | UML and architecture documentation |

---

## 🎯 Project Goal

The goal of this project is to put into practice the design and development of a complete distributed application by combining:

- C++ development;
- software architecture;
- network programming;
- asynchronous communication;
- graphical user interface development;
- database management;
- communication security;
- dependency injection;
- unit testing and mocking;
- containerization with Docker.

The project is developed incrementally, with new features being added as development progresses.

---

## 👥 Project

**Teams** is a project developed in an educational context.