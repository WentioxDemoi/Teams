cmake_minimum_required(VERSION 3.16)
project(server LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

# ------------------------------
# Boost
# ------------------------------
find_package(Boost 1.89 REQUIRED COMPONENTS filesystem thread date_time)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found. Please install Boost 1.89 or higher.")
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
# NATS C
# ------------------------------
if(APPLE)
    find_path(NATS_INCLUDE_DIR nats/nats.h /opt/homebrew/include /usr/local/include)
    find_library(NATS_LIBRARY nats /opt/homebrew/lib /usr/local/lib)
else()
    find_path(NATS_INCLUDE_DIR nats/nats.h /usr/include /usr/local/include)
    find_library(NATS_LIBRARY nats /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib)
endif()

if(NOT NATS_INCLUDE_DIR OR NOT NATS_LIBRARY)
    message(FATAL_ERROR "NATS C library not found. Please install it.")
endif()

# ------------------------------
# Sources
# ------------------------------
file(GLOB_RECURSE Main_SOURCES "src/*.cpp" "src/*.h")
file(GLOB_RECURSE Repositories_SOURCES "src/Repositories/*.cpp" "src/Repositories/*.h")
file(GLOB_RECURSE Database_SOURCES "src/Database/*.cpp" "src/Database/*.h")
file(GLOB_RECURSE Errors_SOURCES "src/Errors/*.cpp" "src/Errors/*.h")
file(GLOB_RECURSE Servers_SOURCES "src/Servers/*.cpp" "src/Servers/*.h")
file(GLOB_RECURSE Services_SOURCES "src/Services/*.cpp" "src/Services/*.h")
file(GLOB_RECURSE Sessions_SOURCES "src/Sessions/*.cpp" "src/Sessions/*.h")
file(GLOB_RECURSE Structs_SOURCES "src/Structs/*.cpp" "src/Structs/*.h")

set(SOURCES
    ${Main_SOURCES}
    ${Repositories_SOURCES}
    ${Database_SOURCES}
    ${Errors_SOURCES}
    ${Servers_SOURCES}
    ${Services_SOURCES}
    ${Sessions_SOURCES}
    ${Structs_SOURCES}
)

# ------------------------------
# Executable
# ------------------------------
add_executable(server ${SOURCES})

# ------------------------------
# Includes par target (propre)
# ------------------------------
target_include_directories(server PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${ARGON2_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${NATS_INCLUDE_DIR}
)

# ------------------------------
# Link libraries
# ------------------------------
target_link_libraries(server PRIVATE
    pqxx
    pq
    Boost::filesystem
    Boost::thread
    Boost::date_time
    pthread
    OpenSSL::SSL
    OpenSSL::Crypto
    ${NATS_LIBRARY}
    ${ARGON2_LIBRARIES}
)

# ------------------------------
# RPATH Boost (Linux / macOS)
# ------------------------------
if(APPLE)
    set(BOOST_LIB_PATH "/opt/homebrew/lib")
elseif(UNIX)
    set(BOOST_LIB_PATH "/usr/local/lib")
endif()

target_link_options(server PRIVATE -Wl,-rpath,${BOOST_LIB_PATH})