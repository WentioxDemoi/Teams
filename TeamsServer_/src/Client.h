#ifndef CORE_H
#define CORE_H

#include <boost/asio.hpp>
#include <iostream>
#include <atomic>

using boost::asio::ip::tcp;

class Client {
public:
    Client(tcp::socket socket_) 
        : socket(std::move(socket_)), running_(true) {}

    void run();
    void stop();

    int id;

private:
    tcp::socket socket;
    std::atomic_bool running_;
};

#endif