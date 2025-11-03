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

# === OpenSSL ===
set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl@3")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_LIBRARIES "${OPENSSL_ROOT_DIR}/lib")

find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR})
message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")

# === Qt ===
set(CMAKE_PREFIX_PATH "$ENV{HOME}/Qt/6.9.3/macos/lib/cmake/Qt6")
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Test)

# === Boost (header-only pour asio) ===
set(BOOST_ROOT "/opt/homebrew/opt/boost")
set(BOOST_INCLUDEDIR "/opt/homebrew/include")
include_directories(${BOOST_INCLUDEDIR})

# === Sources ===
file(GLOB Atoms_SOURCES "Atoms/*.cpp" "Atoms/*.h")
file(GLOB Molecules_SOURCES "Molecules/*.cpp" "Molecules/*.h")
file(GLOB Organisms_SOURCES "Organisms/*.cpp" "Organisms/*.h")
file(GLOB Network_SOURCES "Network/*.cpp" "Network/*.h")

set(SOURCES
    ${Atoms_SOURCES}
    ${Molecules_SOURCES}
    ${Organisms_SOURCES}
    ${Network_SOURCES}
)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ${SOURCES}
)

# === Exécutable principal ===
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TeamsClient_
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(TeamsClient_ ${PROJECT_SOURCES})
endif()

target_link_libraries(TeamsClient_
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        OpenSSL::SSL
        OpenSSL::Crypto
)

if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.TeamsClient_)
endif()
set_target_properties(TeamsClient_ PROPERTIES
    ${BUNDLE_ID_OPTION}
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

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TeamsClient_)
endif()

# === GoogleTest ===
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

# Forcer GoogleTest à utiliser C++20 et arm64
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -arch arm64")

FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB TEST_SOURCES "Tests/*.cpp")

add_executable(TeamsClient_Tests
    ${TEST_SOURCES}
    ${SOURCES}
)

set_target_properties(TeamsClient_Tests PROPERTIES AUTOMOC ON)

target_link_libraries(TeamsClient_Tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
        OpenSSL::SSL
        OpenSSL::Crypto
)

include(GoogleTest)
gtest_discover_tests(TeamsClient_Tests)