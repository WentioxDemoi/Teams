#ifndef AUTHSESSION_H
#define AUTHSESSION_H

#include "../includes.h"
#include "../Errors/ErrorManager.h"
#include "../Services/NatsClient.h"
#include "SessionTools.h"

class AuthSession : public std::enable_shared_from_this<AuthSession>, public ErrorManager {
public:
    AuthSession(tcp::socket socket, ssl::context& ctx, asio::thread_pool& db_pool)
        : stream_(std::move(socket), ctx), db_pool_(db_pool) {}

    void start();

private:
    void do_read();
    void handle_type(std::string payload);
    void handle_login(std::string payload);
    void handle_register(std::string payload);
    void handle_register_response(std::string payload);
    void handle_token(std::string payload);
    void handle_presence(std::string payload);


    ssl::stream<tcp::socket> stream_;
    asio::thread_pool& db_pool_;
    std::array<char, 4096> buffer_;
};

#endif