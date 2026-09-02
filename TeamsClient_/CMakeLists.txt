cmake_minimum_required(VERSION 3.16)
project(TeamsClient_ VERSION 0.1 LANGUAGES CXX)

# === Options globales ===
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -stdlib=libc++")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build the tests" ON)
option(ENABLE_COVERAGE "Enable code coverage" ON)

# === OpenSSL ===
set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl@3")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_LIBRARIES "${OPENSSL_ROOT_DIR}/lib")
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

# === Qt ===
set(CMAKE_PREFIX_PATH "$ENV{HOME}/Qt/6.9.3/macos/")
find_package(Qt6 REQUIRED COMPONENTS
    Core Qml Network Test Sql
    Multimedia MultimediaWidgets
    Quick QuickControls2
)
set(Qt6Keychain_DIR "$ENV{HOME}/Qt/6.9.3/macos/lib/cmake/Qt6Keychain")
find_package(Qt6Keychain REQUIRED)

qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

# === WebRTC ===
set(WEBRTC_DIR "${CMAKE_SOURCE_DIR}/third_party/webrtc")
set(WEBRTC_LIB "${WEBRTC_DIR}/lib/libwebrtc.a")

set(WEBRTC_INCLUDE_DIRS
    ${WEBRTC_DIR}/src
    ${WEBRTC_DIR}/src/api
    ${WEBRTC_DIR}/src/api/rtc_base
    ${WEBRTC_DIR}/src/third_party/abseil-cpp
    ${WEBRTC_DIR}/src/api/video
    ${WEBRTC_DIR}/src/api/media_stream
)

# === Libyuv ===
set(LIBYUV_ROOT "${CMAKE_SOURCE_DIR}/third_party/libyuv")

# === Info.plist ===
set(MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist)

# =========================================================
# Sources
# =========================================================
file(GLOB MAIN_HEADERS      "src/*.h")

file(GLOB_RECURSE MODELS_SOURCES        "src/Models/*.cpp")
file(GLOB_RECURSE MODELS_HEADERS        "src/Models/*.h")

file(GLOB_RECURSE REPOSITORIES_SOURCES  "src/Repositories/*.cpp")
file(GLOB_RECURSE REPOSITORIES_HEADERS  "src/Repositories/*.h")

file(GLOB_RECURSE DATABASE_SOURCES      "src/Database/*.cpp")
file(GLOB_RECURSE DATABASE_HEADERS      "src/Database/*.h")

file(GLOB_RECURSE VIEWMODELS_SOURCES    "src/ViewModels/*.cpp")
file(GLOB_RECURSE VIEWMODELS_HEADERS    "src/ViewModels/*.h")

file(GLOB_RECURSE SERVICES_SOURCES      "src/Services/*.cpp")
file(GLOB_RECURSE SERVICES_HEADERS      "src/Services/*.h")

file(GLOB_RECURSE COMPONENTS_SOURCES    "src/Components/*.cpp")
file(GLOB_RECURSE COMPONENTS_HEADERS    "src/Components/*.h")

file(GLOB_RECURSE UTILS_SOURCES         "src/Utils/*.cpp")
file(GLOB_RECURSE UTILS_HEADERS         "src/Utils/*.h")

file(GLOB_RECURSE CORE_SOURCES          "src/Core/*.cpp")
file(GLOB_RECURSE CORE_HEADERS          "src/Core/*.h")

set(CORE_LIB_SOURCES
    ${MODELS_SOURCES}
    ${REPOSITORIES_SOURCES}
    ${DATABASE_SOURCES}
    ${VIEWMODELS_SOURCES}
    ${SERVICES_SOURCES}
    ${COMPONENTS_SOURCES}
    ${UTILS_SOURCES}
    ${CORE_SOURCES}
)

set(CORE_LIB_HEADERS
    ${MAIN_HEADERS}
    ${MODELS_HEADERS}
    ${REPOSITORIES_HEADERS}
    ${DATABASE_HEADERS}
    ${VIEWMODELS_HEADERS}
    ${SERVICES_HEADERS}
    ${COMPONENTS_HEADERS}
    ${UTILS_HEADERS}
    ${CORE_HEADERS}
)

set(PROJECT_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ViewModels
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Services/Enums
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Components
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Core
)

# === Resources (partagées) ===
qt_add_resources(app_resources resources/resources.qrc)

# =========================================================
# TeamsClient_Core : lib statique "backing library" du module QML
# Contient tout sauf main.cpp -> une seule source de vérité pour les
# includes / defines / link, utilisée par l'appli ET les tests.
# =========================================================
qt_add_library(TeamsClient_Core STATIC
    ${CORE_LIB_SOURCES}
    ${CORE_LIB_HEADERS}
    ${app_resources}
)

qt_add_qml_module(TeamsClient_Core
    URI TeamsClient
    VERSION 1.0
    RESOURCE_PREFIX /qt/qml
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/TeamsClient
    QML_FILES
        src/QML/Main.qml
        src/QML/AuthView.qml
        src/QML/WorkspaceView.qml
        src/QML/LoadingView.qml
        src/QML/HomeView.qml
        src/QML/MessageView.qml
        src/QML/ContactItem.qml
        src/QML/CallWindow.qml
)

target_include_directories(TeamsClient_Core PUBLIC
    ${PROJECT_INCLUDE_DIRS}
    ${LIBYUV_ROOT}/include
)

target_include_directories(TeamsClient_Core SYSTEM PUBLIC
    ${WEBRTC_INCLUDE_DIRS}
)

target_compile_definitions(TeamsClient_Core PUBLIC
    WEBRTC_MAC
    WEBRTC_POSIX
    WEBRTC_ARCH_LITTLE_ENDIAN
    # NDEBUG doit matcher le mode de build de libwebrtc.a (prébuilt en release).
    # RTC_DCHECK_IS_ON() dépend de cette macro : un mismatch entre le code
    # appelant et la lib prébuiltée casse l'ABI (symboles DCHECK absents du .a).
    # PUBLIC pour propager à TeamsClient_ ET TeamsClient_Tests de façon cohérente.
    NDEBUG
)

target_compile_features(TeamsClient_Core PUBLIC cxx_std_20)

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(TeamsClient_Core PRIVATE
        --coverage
        -O0
        -g
    )
endif()

target_link_libraries(TeamsClient_Core PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::QuickControls2
    Qt6Keychain::Qt6Keychain
    OpenSSL::SSL
    OpenSSL::Crypto
    ${LIBYUV_ROOT}/lib/libyuv.a
)

if(APPLE)
    target_link_options(TeamsClient_Core PUBLIC "-Wl,-force_load,${WEBRTC_LIB}")
    target_link_libraries(TeamsClient_Core PUBLIC
        ${WEBRTC_LIB}
        "-framework Foundation"
        "-framework AVFoundation"
        "-framework CoreAudio"
        "-framework AudioToolbox"
        "-framework VideoToolbox"
        "-framework CoreMedia"
        "-framework CoreVideo"
        "-framework IOKit"
        "-framework IOSurface"
        "-framework ApplicationServices"
        "-framework ScreenCaptureKit"
    )
endif()

# =========================================================
# Exécutable principal
# =========================================================
file(GLOB MAIN_SOURCES "src/*.cpp")
list(FILTER MAIN_SOURCES EXCLUDE REGEX ".*main\\.cpp$")
set(APP_MAIN_SOURCE "src/main.cpp")

qt_add_executable(TeamsClient_
    MANUAL_FINALIZATION
    ${APP_MAIN_SOURCE}
)

target_link_libraries(TeamsClient_ PRIVATE
    TeamsClient_Core
    TeamsClient_Coreplugin
)

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(TeamsClient_ PRIVATE
        --coverage
    )
endif()

set_target_properties(TeamsClient_ PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST     ${CMAKE_SOURCE_DIR}/Info.plist
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING
        ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS TeamsClient_
    BUNDLE  DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
qt_finalize_executable(TeamsClient_)

# =========================================================
# Tests
# =========================================================
if(BUILD_TESTS)
    enable_testing()

    file(GLOB_RECURSE TEST_SOURCES "Tests/*.cpp")
    file(GLOB_RECURSE TEST_HEADERS "Tests/*.h")

    add_executable(TeamsClient_Tests
        ${TEST_SOURCES}
        ${TEST_HEADERS}
    )

    set_target_properties(TeamsClient_Tests PROPERTIES AUTOMOC ON)

    target_link_libraries(TeamsClient_Tests
        PRIVATE
        TeamsClient_Core
        TeamsClient_Coreplugin
        Qt${QT_VERSION_MAJOR}::Test
    )

    if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(TeamsClient_Tests PRIVATE
            --coverage
            -O0
            -g
        )

        target_link_options(TeamsClient_Tests PRIVATE
            --coverage
        )
    endif()

    add_test(NAME TeamsClient_Tests COMMAND TeamsClient_Tests)
endif()