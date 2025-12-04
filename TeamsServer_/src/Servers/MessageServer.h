#ifndef MESSAGESERVER_H
#define MESSAGESERVER_H

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/thread_pool.hpp>
#include <iostream>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
namespace ssl = asio::ssl;

class MessageServer {
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