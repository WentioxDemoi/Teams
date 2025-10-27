cmake_minimum_required(VERSION 3.16)
project(server LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

# Chemins Boost
set(BOOST_ROOT /usr/local/boost_1_89_0)
include_directories(${BOOST_ROOT}/include)

add_executable(server src/main.cpp src/Client.cpp src/Client.h)

target_link_libraries(server PRIVATE
    ${BOOST_ROOT}/lib/libboost_filesystem.so
    ${BOOST_ROOT}/lib/libboost_thread.so
    ${BOOST_ROOT}/lib/libboost_date_time.so
    pthread
)

target_link_options(server PRIVATE -Wl,-rpath,${BOOST_ROOT}/lib)