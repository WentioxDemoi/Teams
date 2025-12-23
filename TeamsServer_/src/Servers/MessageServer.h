#ifndef MESSAGESERVER_H
#define MESSAGESERVER_H

#include "../includes.h"
#include "../Errors/ErrorManager.h"

class MessageServer : public ErrorManager {
public:
    MessageServer(asio::io_context& io_context, ssl::context& ssl_ctx,
               tcp::endpoint endpoint, asio::thread_pool& db_pool);

private:
    void do_accept();

    tcp::acceptor acceptor_;
    ssl::context& ssl_ctx_;
    asio::thread_pool& db_pool_;
};

#endif