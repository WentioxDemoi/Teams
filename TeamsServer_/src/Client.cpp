#include "Client.h"
#include <iostream>

void Client::run()
{
    try {
        std::cout << "Client " << id << " connecté (TLS)" << std::endl;

        char data[1024];
        while (running_) {
            boost::system::error_code ec;
            size_t len = socket->read_some(boost::asio::buffer(data), ec);

            if (ec == boost::asio::error::eof) {
                std::cout << "Client " << id << " déconnecté proprement.\n";
                break;
            } else if (ec) {
                std::cerr << "Erreur lecture client " << id << ": " << ec.message() << std::endl;
                break;
            }

            std::string message(data, len);
            std::cout << "Message du client " << id << ": " << message;

            std::string response = "Message reçu (TLS) : Bonjour client n°" + std::to_string(id);
            boost::asio::write(*socket, boost::asio::buffer(response), ec);
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
    if (socket && socket->lowest_layer().is_open()) {
        socket->shutdown(ec);
        socket->lowest_layer().close(ec);
    }
}