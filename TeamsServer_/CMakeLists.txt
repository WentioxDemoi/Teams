cmake_minimum_required(VERSION 3.16)
project(server LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------
# Code coverage
# ------------------------------

option(ENABLE_COVERAGE "Enable code coverage" ON)

if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0 -g)
        add_link_options(--coverage)
    else()
        message(FATAL_ERROR "Coverage is only supported with GCC or Clang.")
    endif()

endif()

# ------------------------------
# Boost
# ------------------------------
find_package(Boost REQUIRED)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found. Please install Boost 1.74 or higher.")
endif()

# ------------------------------
# Argon2 (pkg-config)
# ------------------------------
find_package(PkgConfig REQUIRED)
pkg_check_modules(ARGON2 REQUIRED libargon2)
if(NOT ARGON2_FOUND)
    message(FATAL_ERROR "Argon2 not found. Please install libargon2-dev.")
endif()

# ------------------------------
# OpenSSL
# ------------------------------
find_package(OpenSSL REQUIRED)
if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found.")
endif()

# ------------------------------
# Sources par dossier (GLOB RECURSE auto)
# ------------------------------
file(GLOB_RECURSE Common_SOURCES "src/Common/*.cpp" "src/Common/*.h")
file(GLOB_RECURSE Core_SOURCES "src/Core/*.cpp" "src/Core/*.h")
file(GLOB_RECURSE Infrastructure_SOURCES "src/Infrastructure/*.cpp" "src/Infrastructure/*.h")
file(GLOB_RECURSE Network_SOURCES "src/Network/*.cpp" "src/Network/*.h")
file(GLOB_RECURSE Handlers_SOURCES "src/Handlers/*.cpp" "src/Handlers/*.h")
file(GLOB_RECURSE Utils_SOURCES "src/Utils/*.cpp" "src/Utils/*.h")

set(LIB_SOURCES
    ${Common_SOURCES}
    ${Core_SOURCES}
    ${Infrastructure_SOURCES}
    ${Network_SOURCES}
    ${Handlers_SOURCES}
    ${Utils_SOURCES}
)

# ------------------------------
# Library (réutilisable pour tests)
# ------------------------------
add_library(server_lib STATIC ${LIB_SOURCES})

target_include_directories(server_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/Common
    ${CMAKE_SOURCE_DIR}/src/Core
    ${CMAKE_SOURCE_DIR}/src/Infrastructure
    ${CMAKE_SOURCE_DIR}/src/Network
    ${CMAKE_SOURCE_DIR}/src/Handlers
    ${CMAKE_SOURCE_DIR}/src/Utils
    ${Boost_INCLUDE_DIRS}
    ${ARGON2_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(server_lib PUBLIC
    pqxx
    pq
    pthread
    OpenSSL::SSL
    OpenSSL::Crypto
    ${ARGON2_LIBRARIES}
)

# ------------------------------
# Executable (main + lib)
# ------------------------------
set(MAIN_SOURCES
    src/main.cpp
    src/AppCompositionRoot.cpp
    src/AppCompositionRoot.h
)

add_executable(server ${MAIN_SOURCES})
target_link_libraries(server PRIVATE server_lib)

# ------------------------------
# Tests
# ------------------------------

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE TEST_SOURCES "Tests/*.cpp")
add_executable(server_tests ${TEST_SOURCES})

target_link_libraries(server_tests PRIVATE
    server_lib
    gtest
    gtest_main
    gmock
)

include(GoogleTest)
gtest_discover_tests(server_tests)


