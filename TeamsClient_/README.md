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

```bash
mkdir build && cd build
cmake ..
cmake --build .
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
