cmake_minimum_required(VERSION 3.16)
project(server LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

set(BOOST_ROOT /usr/local/boost_1_89_0)
include_directories(${BOOST_ROOT}/include)

include_directories(/opt/homebrew/include)

find_package(OpenSSL REQUIRED)

file(GLOB Main_SOURCES "src/*.cpp" "src/*.h")
file(GLOB Database_SOURCES "src/Database/*.cpp" "src/Database/*.h")

set(SOURCES
    ${Main_SOURCES} 
    ${Database_SOURCES}
)

add_executable(server ${SOURCES})

target_link_libraries(server PRIVATE
    pqxx
    pq
    ${BOOST_ROOT}/lib/libboost_filesystem.so
    ${BOOST_ROOT}/lib/libboost_thread.so
    ${BOOST_ROOT}/lib/libboost_date_time.so
    pthread
    OpenSSL::SSL
    OpenSSL::Crypto
    /opt/homebrew/lib/libnats.dylib
)

target_link_options(server PRIVATE -Wl,-rpath,${BOOST_ROOT}/lib)