#ifndef CLIENT_H
#define CLIENT_H

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <memory>
#include <atomic>

namespace ssl = boost::asio::ssl;
using boost::asio::ip::tcp;

class Client {
public:
    explicit Client(std::shared_ptr<ssl::stream<tcp::socket>> socket)
        : socket(std::move(socket)), running_(true), id(0) {}

    void run();
    void stop();

    int id;

private:
    std::shared_ptr<ssl::stream<tcp::socket>> socket;
    std::atomic<bool> running_;
};

#endif