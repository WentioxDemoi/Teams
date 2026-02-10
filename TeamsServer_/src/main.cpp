#include "Handlers/Handler.h"
#include "Infrastructure/Database/DatabaseManager.h"
#include "Network/Server/TcpListener.h"
#include "Network/Session/AuthSession.h"
#include "includes.h"

int main() {
  try {
    const int db_threads = 4;
    const int auth_threads = 1; // Au moins 2 pour le serveur

    // Pool de threads pour les opérations DB
    // asio::thread_pool db_pool(db_threads);

    // Contexte ASIO pour le serveur auth
    asio::io_context auth_io;

    // Configuration SSL
    ssl::context ssl_ctx(ssl::context::tlsv12_server);
    ssl_ctx.use_certificate_chain_file("server.crt");
    ssl_ctx.use_private_key_file("server.key", ssl::context::pem);

    // Création du serveur (lance do_accept automatiquement)
    TcpListener<AuthSession> auth_server(auth_io, ssl_ctx,
                                         tcp::endpoint(tcp::v4(), 8080));

    std::cout << "Server running:\n"
              << "- Auth on 8080 with " << auth_threads << " threads\n"
              << "- DB pool threads: " << db_threads << "\n";

    std::vector<std::thread> threads;
    for (int i = 0; i < auth_threads; ++i) {
      threads.emplace_back([&auth_io]() { auth_io.run(); });
    }

    // Attendre que tous les threads se terminent
    for (auto &t : threads) {
      t.join();
    }

    // Attendre que le pool DB se termine

  } catch (const std::exception &e) {
    std::cerr << "Server error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}