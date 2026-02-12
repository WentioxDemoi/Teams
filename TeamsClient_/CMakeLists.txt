cmake_minimum_required(VERSION 3.16)
project(TeamsClient_ VERSION 0.1 LANGUAGES CXX)

# === Options globales ===
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_OSX_ARCHITECTURES "arm64")   # Pour Apple Silicon
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++") # STL correcte pour macOS
option(BUILD_TESTS "Build the tests" OFF)

# === OpenSSL ===
set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl@3")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_LIBRARIES "${OPENSSL_ROOT_DIR}/lib")
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")

# === Qt ===
set(CMAKE_PREFIX_PATH "$ENV{HOME}/Qt/6.9.3/macos/")  # Root of Qt install
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Test Sql Multimedia MultimediaWidgets)
set(Qt6Keychain_DIR "$ENV{HOME}/Qt/6.9.3/macos/lib/cmake/Qt6Keychain")
find_package(Qt6Keychain REQUIRED)

# === Info.plist ===
set(MACOSX_BUNDLE_INFO_PLIST
    ${CMAKE_SOURCE_DIR}/Info.plist
)

# === Collecte automatique des sources par catégorie ===
# Models
file(GLOB_RECURSE MAIN_SOURCES "src/*.cpp")
file(GLOB_RECURSE MAIN_HEADERS "src/*.h")

# Models
file(GLOB_RECURSE MODELS_SOURCES "src/Models/*.cpp")
file(GLOB_RECURSE MODELS_HEADERS "src/Models/*.h")

# Repositorie
file(GLOB_RECURSE REPOSITORIES_SOURCES "src/Repositories/*.cpp")
file(GLOB_RECURSE REPOSITORIES_HEADERS "src/Repositories/*.h")

# Database
file(GLOB_RECURSE DATABASE_SOURCES "src/Database/*.cpp")
file(GLOB_RECURSE DATABASE_HEADERS "src/Database/*.h")

# ViewModels
file(GLOB_RECURSE VIEWMODELS_SOURCES "src/ViewModels/*.cpp")
file(GLOB_RECURSE VIEWMODELS_HEADERS "src/ViewModels/*.h")

# Views
file(GLOB_RECURSE VIEWS_SOURCES "src/Views/*.cpp")
file(GLOB_RECURSE VIEWS_HEADERS "src/Views/*.h")

# Services
file(GLOB_RECURSE SERVICES_SOURCES "src/Services/*.cpp")
file(GLOB_RECURSE SERVICES_HEADERS "src/Services/*.h")

# Components
file(GLOB_RECURSE COMPONENTS_SOURCES "src/Components/*.cpp")
file(GLOB_RECURSE COMPONENTS_HEADERS "src/Components/*.h")

# Utils
file(GLOB_RECURSE UTILS_SOURCES "src/Utils/*.cpp")
file(GLOB_RECURSE UTILS_HEADERS "src/Utils/*.h")

# Core
file(GLOB_RECURSE CORE_SOURCES "src/Core/*.cpp")
file(GLOB_RECURSE CORE_HEADERS "src/Core/*.h")

# Regroupement de toutes les sources
set(ALL_SOURCES
    ${MAIN_SOURCES}
    ${MODELS_SOURCES}
    ${REPOSITORIES_SOURCES}
    ${DATABASE_SOURCES}
    ${VIEWMODELS_SOURCES}
    ${VIEWS_SOURCES}
    ${SERVICES_SOURCES}
    ${COMPONENTS_SOURCES}
    ${UTILS_SOURCES}
    ${CORE_SOURCES}
)

set(ALL_HEADERS
    ${MAIN_HEADERS}
    ${MODELS_HEADERS}
    ${REPOSITORIES_HEADERS}
    ${DATABASE_SOURCES}
    ${VIEWMODELS_HEADERS}
    ${VIEWS_HEADERS}
    ${SERVICES_HEADERS}
    ${COMPONENTS_HEADERS}
    ${UTILS_HEADERS}
    ${CORE_HEADERS}
)

set(PROJECT_SOURCES
    ${ALL_SOURCES}
    ${ALL_HEADERS}
    ${VIEWS_UI}
)

qt_add_resources(app_resources
    resources/resources.qrc
)

qt_add_executable(TeamsClient_
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${app_resources}
)

# Affichage des fichiers trouvés (debug)
message(STATUS "Models sources: ${MODELS_SOURCES}")
message(STATUS "ViewModels sources: ${VIEWMODELS_SOURCES}")
message(STATUS "Views sources: ${VIEWS_SOURCES}")
message(STATUS "Services sources: ${SERVICES_SOURCES}")
message(STATUS "Components sources: ${COMPONENTS_SOURCES}")
message(STATUS "Utils sources: ${UTILS_SOURCES}")
message(STATUS "Core sources: ${CORE_SOURCES}")
message(STATUS "UI files: ${VIEWS_UI}")


# Include directories pour l'accès aux headers
target_include_directories(TeamsClient_ PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ViewModels
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Views
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Components
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Core
)

target_link_libraries(TeamsClient_
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Multimedia 
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    Qt6Keychain::Qt6Keychain
    OpenSSL::SSL
    OpenSSL::Crypto
)

get_target_property(QT6_VERSION Qt6::Core VERSION)
if(QT6_VERSION VERSION_LESS "6.1.0")
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.TeamsClient_)
endif()

set_target_properties(TeamsClient_ PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS TeamsClient_
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
qt_finalize_executable(TeamsClient_)



if(BUILD_TESTS)
    enable_testing()

    # Collecte des tests
    file(GLOB_RECURSE TEST_SOURCES "Tests/*.cpp")
    file(GLOB_RECURSE TEST_HEADERS "Tests/*.h")

    # Exclure le main.cpp des sources pour les tests
    list(FILTER ALL_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

    add_executable(TeamsClient_Tests
        ${TEST_SOURCES}
        ${TEST_HEADERS}
        ${ALL_SOURCES}
        ${ALL_HEADERS}
    )

    set_target_properties(TeamsClient_Tests PROPERTIES AUTOMOC ON)

    target_include_directories(TeamsClient_Tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Models
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ViewModels
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Views
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Services
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Components
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core
    )

    target_link_libraries(TeamsClient_Tests
        PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Multimedia 
        Qt${QT_VERSION_MAJOR}::MultimediaWidgets
        Qt6Keychain::Qt6Keychain
        OpenSSL::SSL
        OpenSSL::Crypto
    )
    add_test(NAME TeamsClient_Tests COMMAND TeamsClient_Tests)
endif()

# === Messages de résumé ===
message(STATUS "")
message(STATUS "═══════════════════════════════════════════")
message(STATUS "  Configuration du projet TeamsClient_")
message(STATUS "═══════════════════════════════════════════")
message(STATUS "Qt version: ${QT_VERSION}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Architecture: ${CMAKE_OSX_ARCHITECTURES}")
message(STATUS "Nombre de fichiers sources: ${CMAKE_MATCH_COUNT}")
message(STATUS "  - Models: ${CMAKE_MATCH_0}")
message(STATUS "  - ViewModels: ${CMAKE_MATCH_1}")
message(STATUS "  - Views: ${CMAKE_MATCH_2}")
message(STATUS "  - Services: ${CMAKE_MATCH_3}")
message(STATUS "═══════════════════════════════════════════")
message(STATUS "")