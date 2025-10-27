#include <thread>
#include <mutex>
#include <map>
#include <iostream>
#include "Client.h"

using boost::asio::ip::tcp;

void handleClient(std::shared_ptr<Client> client, std::mutex *mutex,
                  std::map<std::thread::id, std::shared_ptr<Client>>* list, int nb)
{
    std::cout << "Nouvelle connexion" << std::endl;
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
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));

        std::cout << "Serveur actif sur le port 12345" << std::endl;

        for (;;) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            nb_client++;
            auto client = std::make_shared<Client>(std::move(socket));

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