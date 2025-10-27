#include "Client.h"

void Client::run()
{
    try {
        std::cout << "Client " << id << " connecté depuis "
                  << socket.remote_endpoint() << std::endl;

        char data[1024];
        while (running_) {
            boost::system::error_code ec;
            size_t len = socket.read_some(boost::asio::buffer(data), ec);

            if (ec == boost::asio::error::eof) {
                std::cout << "Client " << id << " déconnecté proprement.\n";
                break;
            } else if (ec) {
                std::cerr << "Erreur lecture client " << id << ": " << ec.message() << std::endl;
                break;
            }

            std::string message(data, len);
            std::cout << "Message du client " << id << ": " << message;

            boost::asio::write(socket, boost::asio::buffer("Message reçu : Bonjour client n°"  + std::to_string(id)), ec);
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception client " << id << ": " << e.what() << std::endl;
    }

    stop();
}

void Client::stop()
{
    running_ = false;
    boost::system::error_code ec;
    if (socket.is_open()) {
        socket.shutdown(tcp::socket::shutdown_both, ec);
        socket.close(ec);
    }
}