#ifndef AUTHSESSION_H
#define AUTHSESSION_H

#include "../includes.h"
#include "../Errors/ErrorManager.h"


class AuthSession : public std::enable_shared_from_this<AuthSession>, public ErrorManager {
public:
    AuthSession(tcp::socket socket, ssl::context& ctx, asio::thread_pool& db_pool)
        : stream_(std::move(socket), ctx),
          db_pool_(db_pool) {}

    void start();

private:
    void do_read();

    ssl::stream<tcp::socket> stream_;
    asio::thread_pool& db_pool_;
    std::array<char, 4096> buffer_;
};

#endif