#include <thread>
#include <mutex>
#include <map>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "Client.h"

using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;

void handleClient(std::shared_ptr<Client> client, std::mutex *mutex,
                  std::map<std::thread::id, std::shared_ptr<Client>>* list, int nb)
{
    std::cout << "Nouvelle connexion sécurisée" << std::endl;
    client->id = nb;
    client->run();
    std::lock_guard<std::mutex> lock(*mutex);
    list->erase(std::this_thread::get_id());
    std::cout << "Client déconnecté, thread supprimé" << std::endl;
}

int main() {
    try {
        std::mutex client_mutex;
        std::map<std::thread::id, std::shared_ptr<Client>> list;
        int nb_client = 0;

        boost::asio::io_context io_context;
        ssl::context ctx(ssl::context::tlsv12_server);

        ctx.use_certificate_chain_file("server.crt");
        ctx.use_private_key_file("server.key", ssl::context::pem);

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));

        std::cout << "Serveur TLS actif sur le port 12345" << std::endl;

        for (;;) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            auto ssl_socket = std::make_shared<ssl::stream<tcp::socket>>(std::move(socket), ctx);

            boost::system::error_code ec;
            ssl_socket->handshake(ssl::stream_base::server, ec);

            if (ec) {
                std::cerr << "Erreur handshake TLS : " << ec.message() << std::endl;
                continue;
            }

            nb_client++;
            auto client = std::make_shared<Client>(ssl_socket);

            std::thread th(handleClient, client, &client_mutex, &list, nb_client);
            {
                std::lock_guard<std::mutex> lock(client_mutex);
                list[th.get_id()] = client;
            }
            th.detach();
        }
    } catch (const std::exception& e) {
        std::cerr << "Erreur serveur : " << e.what() << std::endl;
    }
}