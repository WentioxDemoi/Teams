#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <thread>
#include <mutex>
#include <map>
#include <iostream>

#include "Session.h"

using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;

class Server {
    public :
        Server(boost::asio::io_context &io_context, ssl::context &ssl_ctx, const tcp::endpoint &endpoint, boost::asio::thread_pool &db_pool) 
: io_context_(io_context),
  ssl_ctx_(ssl_ctx),
  acceptor_(io_context, endpoint.protocol()),
  db_pool_(db_pool)
{
    boost::system::error_code ec;

    acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
    if (ec) throw std::runtime_error("Set option: " + ec.message());

    acceptor_.bind(endpoint, ec);
    if (ec) throw std::runtime_error("Binding acceptor: " + ec.message());

    acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec) throw std::runtime_error("Listen acceptor: " + ec.message());
};
        void Core();
    private : 
    

    
    ssl::context &ssl_ctx_;
    tcp::acceptor acceptor_;
    boost::asio::thread_pool &db_pool_;
    boost::asio::io_context &io_context_;
            
};

#endif