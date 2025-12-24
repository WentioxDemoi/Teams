#!/bin/bash

# Script de création de l'architecture MVVM pour Qt Client
# Usage: ./create_mvvm_structure.sh [nom_du_projet]

PROJECT_NAME=${1:-"QtChatClient"}

echo "Création de l'architecture MVVM pour $PROJECT_NAME..."

# Création de la structure de dossiers
mkdir -p "$PROJECT_NAME"/{src,resources,tests}

# Structure src/
mkdir -p "$PROJECT_NAME"/src/{Models,ViewModels,Views,Services,Components,Utils,Core}
mkdir -p "$PROJECT_NAME"/src/Views/{Auth,Chat}

# Structure resources/
mkdir -p "$PROJECT_NAME"/resources/{qss,icons,images}

# Structure tests/
mkdir -p "$PROJECT_NAME"/tests/{ModelTests,ViewModelTests,ServiceTests}

# Création des fichiers Models
touch "$PROJECT_NAME"/src/Models/{User.h,User.cpp}
touch "$PROJECT_NAME"/src/Models/{Message.h,Message.cpp}
touch "$PROJECT_NAME"/src/Models/{Conversation.h,Conversation.cpp}

# Création des fichiers ViewModels
touch "$PROJECT_NAME"/src/ViewModels/{AuthViewModel.h,AuthViewModel.cpp}
touch "$PROJECT_NAME"/src/ViewModels/{ChatViewModel.h,ChatViewModel.cpp}
touch "$PROJECT_NAME"/src/ViewModels/{MainViewModel.h,MainViewModel.cpp}

# Création des fichiers Views
touch "$PROJECT_NAME"/src/Views/{MainWindow.h,MainWindow.cpp,MainWindow.ui}
touch "$PROJECT_NAME"/src/Views/Auth/{AuthView.h,AuthView.cpp,AuthView.ui}
touch "$PROJECT_NAME"/src/Views/Auth/{LoginForm.h,LoginForm.cpp,LoginForm.ui}
touch "$PROJECT_NAME"/src/Views/Auth/{RegisterForm.h,RegisterForm.cpp,RegisterForm.ui}
touch "$PROJECT_NAME"/src/Views/Chat/{ChatView.h,ChatView.cpp,ChatView.ui}
touch "$PROJECT_NAME"/src/Views/Chat/{ConversationList.h,ConversationList.cpp,ConversationList.ui}
touch "$PROJECT_NAME"/src/Views/Chat/{MessageList.h,MessageList.cpp,MessageList.ui}

# Création des fichiers Services
touch "$PROJECT_NAME"/src/Services/{NetworkService.h,NetworkService.cpp}
touch "$PROJECT_NAME"/src/Services/{AuthService.h,AuthService.cpp}
touch "$PROJECT_NAME"/src/Services/{MessageService.h,MessageService.cpp}
touch "$PROJECT_NAME"/src/Services/{WebSocketService.h,WebSocketService.cpp}

# Création des fichiers Components
touch "$PROJECT_NAME"/src/Components/{CustomButton.h,CustomButton.cpp}
touch "$PROJECT_NAME"/src/Components/{CustomLineEdit.h,CustomLineEdit.cpp}
touch "$PROJECT_NAME"/src/Components/{MessageBubble.h,MessageBubble.cpp}
touch "$PROJECT_NAME"/src/Components/{Avatar.h,Avatar.cpp}

# Création des fichiers Utils
touch "$PROJECT_NAME"/src/Utils/{Constants.h}
touch "$PROJECT_NAME"/src/Utils/{Validators.h,Validators.cpp}
touch "$PROJECT_NAME"/src/Utils/{JsonHelper.h,JsonHelper.cpp}

# Création des fichiers Core
touch "$PROJECT_NAME"/src/Core/{Application.h,Application.cpp}
touch "$PROJECT_NAME"/src/Core/{ServiceLocator.h,ServiceLocator.cpp}

# Création du main.cpp
touch "$PROJECT_NAME"/src/main.cpp

# Création des fichiers de ressources
touch "$PROJECT_NAME"/resources/qss/styles.qss

# Création du CMakeLists.txt principal
cat > "$PROJECT_NAME"/CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.16)
project(QtChatClient VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Recherche des packages Qt
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Network
    WebSockets
)

# Sources
set(SOURCES
    src/main.cpp
    
    # Models
    src/Models/User.cpp
    src/Models/Message.cpp
    src/Models/Conversation.cpp
    
    # ViewModels
    src/ViewModels/AuthViewModel.cpp
    src/ViewModels/ChatViewModel.cpp
    src/ViewModels/MainViewModel.cpp
    
    # Views
    src/Views/MainWindow.cpp
    src/Views/Auth/AuthView.cpp
    src/Views/Auth/LoginForm.cpp
    src/Views/Auth/RegisterForm.cpp
    src/Views/Chat/ChatView.cpp
    src/Views/Chat/ConversationList.cpp
    src/Views/Chat/MessageList.cpp
    
    # Services
    src/Services/NetworkService.cpp
    src/Services/AuthService.cpp
    src/Services/MessageService.cpp
    src/Services/WebSocketService.cpp
    
    # Components
    src/Components/CustomButton.cpp
    src/Components/CustomLineEdit.cpp
    src/Components/MessageBubble.cpp
    src/Components/Avatar.cpp
    
    # Utils
    src/Utils/Validators.cpp
    src/Utils/JsonHelper.cpp
    
    # Core
    src/Core/Application.cpp
    src/Core/ServiceLocator.cpp
)

# Headers
set(HEADERS
    # Models
    src/Models/User.h
    src/Models/Message.h
    src/Models/Conversation.h
    
    # ViewModels
    src/ViewModels/AuthViewModel.h
    src/ViewModels/ChatViewModel.h
    src/ViewModels/MainViewModel.h
    
    # Views
    src/Views/MainWindow.h
    src/Views/Auth/AuthView.h
    src/Views/Auth/LoginForm.h
    src/Views/Auth/RegisterForm.h
    src/Views/Chat/ChatView.h
    src/Views/Chat/ConversationList.h
    src/Views/Chat/MessageList.h
    
    # Services
    src/Services/NetworkService.h
    src/Services/AuthService.h
    src/Services/MessageService.h
    src/Services/WebSocketService.h
    
    # Components
    src/Components/CustomButton.h
    src/Components/CustomLineEdit.h
    src/Components/MessageBubble.h
    src/Components/Avatar.h
    
    # Utils
    src/Utils/Constants.h
    src/Utils/Validators.h
    src/Utils/JsonHelper.h
    
    # Core
    src/Core/Application.h
    src/Core/ServiceLocator.h
)

# UI Files
set(UI_FILES
    src/Views/MainWindow.ui
    src/Views/Auth/AuthView.ui
    src/Views/Auth/LoginForm.ui
    src/Views/Auth/RegisterForm.ui
    src/Views/Chat/ChatView.ui
    src/Views/Chat/ConversationList.ui
    src/Views/Chat/MessageList.ui
)

# Création de l'exécutable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

# Link des bibliothèques Qt
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::WebSockets
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
EOF

# Création d'un .gitignore
cat > "$PROJECT_NAME"/.gitignore << 'EOF'
# Build directories
build/
cmake-build-*/

# Qt
*.pro.user
*.autosave
moc_*.cpp
ui_*.h
qrc_*.cpp

# IDEs
.vscode/
.idea/
*.swp
*.swo
*~

# Compiled Object files
*.o
*.obj

# Executables
*.exe
*.out
*.app
EOF

# Création d'un README
cat > "$PROJECT_NAME"/README.md << 'EOF'
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
EOF

echo "✅ Architecture MVVM créée avec succès dans le dossier '$PROJECT_NAME'!"
echo ""
echo "Prochaines étapes:"
echo "  cd $PROJECT_NAME"
echo "  mkdir build && cd build"
echo "  cmake .."
echo "  cmake --build ."