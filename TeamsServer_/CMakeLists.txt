cmake_minimum_required(VERSION 3.16)
project(server LANGUAGES C CXX)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------
# Code coverage
# ------------------------------
option(ENABLE_COVERAGE "Enable code coverage" ON)
if(ENABLE_COVERAGE AND NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(FATAL_ERROR "Coverage is only supported with GCC or Clang.")
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

set(LIB_INCLUDE_DIRS
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

set(LIB_LINK_LIBRARIES
    pqxx
    pq
    pthread
    OpenSSL::SSL
    OpenSSL::Crypto
    ${ARGON2_LIBRARIES}
)

# ------------------------------
# Library (utilisée par l'exécutable "server")
# ------------------------------
add_library(server_lib STATIC ${LIB_SOURCES})
target_include_directories(server_lib PUBLIC ${LIB_INCLUDE_DIRS})
target_link_libraries(server_lib PUBLIC ${LIB_LINK_LIBRARIES})

# ------------------------------
# Library dédiée coverage (utilisée uniquement par server_tests)
# ------------------------------
if(ENABLE_COVERAGE)
    add_library(server_lib_coverage STATIC ${LIB_SOURCES})
    target_include_directories(server_lib_coverage PUBLIC ${LIB_INCLUDE_DIRS})
    target_link_libraries(server_lib_coverage PUBLIC ${LIB_LINK_LIBRARIES})
    target_compile_options(server_lib_coverage PRIVATE --coverage -O0 -g)
    target_link_options(server_lib_coverage PUBLIC --coverage)
endif()

# ------------------------------
# Executable (main + lib)
# Jamais instrumentée : ne produit jamais de .gcda.
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

if(ENABLE_COVERAGE)
    target_link_libraries(server_tests PRIVATE
        server_lib_coverage
        gtest
        gtest_main
        gmock
    )
    target_compile_options(server_tests PRIVATE --coverage -O0 -g)
    target_link_options(server_tests PRIVATE --coverage)
else()
    target_link_libraries(server_tests PRIVATE
        server_lib
        gtest
        gtest_main
        gmock
    )
endif()

include(GoogleTest)
gtest_discover_tests(server_tests)

if(ENABLE_COVERAGE)
    add_custom_target(clean_coverage_data ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Coverage: purge des .gcda avant build"
        COMMAND find ${CMAKE_BINARY_DIR} -name "*.gcda" -delete
        COMMENT "Coverage: purge des .gcda avant build"
    )
    add_dependencies(server_tests clean_coverage_data)
endif()