#include "Database/Database.h"
#include "Servers/AuthServer.h"
#include "Servers/MessageServer.h"
#include "Services/AuthService.h"
#include "Services/NatsClient.h"

#include "includes.h"

int main() {
  try {
    const int db_threads = 4;
    const int auth_threads = 1;
    const int message_threads = 2;

    asio::thread_pool db_pool(db_threads);
    ssl::context ssl_ctx(ssl::context::tlsv12_server);
    ssl_ctx.use_certificate_chain_file("server.crt");
    ssl_ctx.use_private_key_file("server.key", ssl::context::pem);

    asio::io_context auth_io;
    // asio::io_context message_io;

    AuthService authService(
        db_pool); // On instancie le servie pour qu'il se connecte bien à NATS
                  // (en sachant que NatsClient est un singleton static donc
                  // initialisé dès le lancement du binaire)

    // Next step -> Avoir un sertificat ssl par server
    // Peut être avoir plusieurs acceptors en fonction de la demande (On en a un
    // par server pour le moment).
    AuthServer auth_server(auth_io, ssl_ctx, tcp::endpoint(tcp::v4(), 8080),
                           db_pool);
    // MessageServer message_server(message_io, ssl_ctx,
    // tcp::endpoint(tcp::v4(), 8081), db_pool);

    std::vector<std::thread> threads;
    for (int i = 0; i < auth_threads; ++i)
      threads.emplace_back([&] { auth_io.run(); });
    // for(int i=0; i < message_threads; ++i) threads.emplace_back([&]{
    // message_io.run(); });

    std::cout
        << "Server running:\n"
        << "- Auth on 8000 with " << auth_threads
        << " threads\n"
        //   << "- Message on 8001 with " << message_threads << " threads\n"
        << "- DB pool threads: " << db_threads << "\n";

    for (auto &t : threads)
      t.join();
    db_pool.join();
  } catch (const std::exception &e) {
    std::cerr << "Server error: " << e.what() << "\n";
  }
}