#include "Server.h"

void Server::Core()
{
    acceptor_.async_accept([this](const boost::system::error_code& ec, tcp::socket socket) {
        if (!ec) {
            auto session = std::make_shared<Session>(
                std::move(socket), ssl_ctx_, io_context_, db_pool_);
            session->start();
        } else {
            std::cerr << "Accept error: " << ec.message() << "\n";
        }
        Core();
    });
}
