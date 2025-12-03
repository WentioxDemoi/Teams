#include "Server.h"




int main() {
    try {
        const int io_threads = std::max(1u, std::thread::hardware_concurrency());
        const int db_threads = 4;

        boost::asio::io_context io_context;
        boost::asio::thread_pool db_pool(db_threads);
        tcp::endpoint endpoint(tcp::v4(), 12345);
        ssl::context ctx(ssl::context::tlsv12_server);
        ctx.use_certificate_chain_file("server.crt");
        ctx.use_private_key_file("server.key", ssl::context::pem);
        std::unique_ptr<Server> Server_ = std::make_unique<Server>(io_context, ctx, endpoint, db_pool);
        Server_->Core();
        std::vector<std::thread> workers;
        for (int i = 0; i < io_threads; ++i)
            workers.emplace_back([&io_context]() { io_context.run(); });

        std::cout << "Async TLS server running on port 12345 with "
                  << io_threads << " io threads and "
                  << db_threads << " db threads.\n";

        for (auto& t : workers) t.join();
        db_pool.join();
    } catch (const std::exception& e) {
        std::cerr << "Erreur serveur : " << e.what() << std::endl;
    }
}