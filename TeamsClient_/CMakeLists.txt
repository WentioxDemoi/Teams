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
option(BUILD_TESTS "Build the tests" OFF)

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

# === WebRTC ===
set(WEBRTC_DIR "${CMAKE_SOURCE_DIR}/third_party/webrtc")
set(WEBRTC_LIB "${WEBRTC_DIR}/lib/libwebrtc.a")

# === Libyuv ===
set(LIBYUV_ROOT "${CMAKE_SOURCE_DIR}/third_party/libyuv")

# === Info.plist ===
set(MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist)

# =========================================================
# Sources
# =========================================================
file(GLOB MAIN_SOURCES      "src/*.cpp")
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

set(ALL_SOURCES
    ${MAIN_SOURCES}
    ${MODELS_SOURCES}
    ${REPOSITORIES_SOURCES}
    ${DATABASE_SOURCES}
    ${VIEWMODELS_SOURCES}
    ${SERVICES_SOURCES}
    ${COMPONENTS_SOURCES}
    ${UTILS_SOURCES}
    ${CORE_SOURCES}
)

set(ALL_HEADERS
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

set(PROJECT_SOURCES
    ${ALL_SOURCES}
    ${ALL_HEADERS}
)

# === Resources ===
qt_add_resources(app_resources resources/resources.qrc)

# === Exécutable principal ===
qt_add_executable(TeamsClient_
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${app_resources}
)

# === Module QML ===

#Pour éviter les warnings
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

qt_add_qml_module(TeamsClient_
    URI TeamsClient
    VERSION 1.0
    RESOURCE_PREFIX /qt/qml
    QML_FILES
        src/QML/Main.qml
        src/QML/AuthView.qml
        src/QML/WorkspaceView.qml
        src/QML/LoadingView.qml
        src/QML/HomeView.qml
        src/QML/MessageView.qml
        src/QML/ContactItem.qml
)

# === WebRTC include dirs ===
set(WEBRTC_INCLUDE_DIRS
    ${WEBRTC_DIR}/src
    ${WEBRTC_DIR}/src/api
    ${WEBRTC_DIR}/src/api/rtc_base
    ${WEBRTC_DIR}/src/third_party/abseil-cpp
    ${WEBRTC_DIR}/src/api/video
    ${WEBRTC_DIR}/src/api/media_stream
)

target_include_directories(TeamsClient_ PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ViewModels
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Components
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Core
    ${LIBYUV_ROOT}/include
)

target_include_directories(TeamsClient_ SYSTEM PRIVATE
    ${WEBRTC_INCLUDE_DIRS}
)

target_compile_definitions(TeamsClient_ PRIVATE
    WEBRTC_MAC
    WEBRTC_POSIX
    WEBRTC_ARCH_LITTLE_ENDIAN
    NDEBUG
)

target_compile_features(TeamsClient_ PRIVATE cxx_std_20)

if(APPLE)
    target_link_options(TeamsClient_ PRIVATE "-Wl,-force_load,${WEBRTC_LIB}")
endif()

target_link_libraries(TeamsClient_
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::QuickControls2
    Qt6Keychain::Qt6Keychain
    ${WEBRTC_DIR}/lib/libwebrtc.a
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
    OpenSSL::SSL
    OpenSSL::Crypto
    ${LIBYUV_ROOT}/lib/libyuv.a
)

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

    # Copie de ALL_SOURCES sans main.cpp pour les tests
    set(TEST_APP_SOURCES ${ALL_SOURCES})
    list(FILTER TEST_APP_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

    add_executable(TeamsClient_Tests
        ${TEST_SOURCES}
        ${TEST_HEADERS}
        ${TEST_APP_SOURCES}
        ${ALL_HEADERS}
    )

    set_target_properties(TeamsClient_Tests PROPERTIES AUTOMOC ON)

    target_compile_definitions(TeamsClient_Tests PRIVATE
        WEBRTC_MAC
        WEBRTC_POSIX
        WEBRTC_ARCH_LITTLE_ENDIAN
    )

    target_include_directories(TeamsClient_Tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Models
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ViewModels
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Services
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Components
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core
    )

    target_link_libraries(TeamsClient_Tests
        PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
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