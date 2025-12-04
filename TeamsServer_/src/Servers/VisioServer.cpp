#include "VisioServer.h"

VisioServer::VisioServer(asio::io_context& io_context, ssl::context& ssl_ctx,
                         tcp::endpoint endpoint, asio::thread_pool& db_pool)
    : acceptor_(io_context, endpoint), ssl_ctx_(ssl_ctx), db_pool_(db_pool)
{
    do_accept();
}

void VisioServer::do_accept() {
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket){
        if(!ec){
            std::cout << "[Visio] New connection\n";
            // Lancer lecture/écriture Visio
        }
        do_accept();
    });
}