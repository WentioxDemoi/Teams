#include "Infrastructure/Database/DatabaseManager.h"
#include "Network/Server/TcpListenerAuth.h"
#include "Network/Server/TcpListenerWebRTC.h"
#include "Network/Session/AuthSession.h"
#include "includes.h"

int main() {
    std::cout << "[MAIN] Server starting...\n";

    try {
        const int db_threads = 4;
        const int auth_threads = 1; // Au moins 2 pour le serveur
        const int webrtc_threads = 1;

        std::cout << "[MAIN] Configuration:\n";
        std::cout << "  - DB threads: " << db_threads << "\n";
        std::cout << "  - Auth threads: " << auth_threads << "\n";

        // Contexte ASIO
        std::cout << "[MAIN] Initializing ASIO contexts...\n";
        asio::io_context auth_io;
        asio::io_context webrtc_io;

        // Configuration SSL
        std::cout << "[MAIN] Initializing SSL context...\n";
        ssl::context ssl_ctx(ssl::context::tlsv12_server);

        std::cout << "[MAIN] Loading SSL certificate...\n";
        ssl_ctx.use_certificate_chain_file("server.crt");

        std::cout << "[MAIN] Loading SSL private key...\n";
        ssl_ctx.use_private_key_file("server.key", ssl::context::pem);

        // Création des serveurs
        std::cout << "[MAIN] Starting Auth TCP listener on port 8080...\n";
        TcpListenerAuth auth_server(
            auth_io,
            ssl_ctx,
            tcp::endpoint(tcp::v4(), 8080)
        );

        std::cout << "[MAIN] Starting WebRTC TCP listener on port 8081...\n";
        TcpListenerWebRTC webrtc_server(
            webrtc_io,
            ssl_ctx,
            tcp::endpoint(tcp::v4(), 8081)
        );

        std::cout << "[MAIN] Server running:\n";
        std::cout << "  - Auth:   port 8080 (" << auth_threads << " threads)\n";
        std::cout << "  - WebRTC: port 8081\n";
        std::cout << "  - DB pool threads: " << db_threads << "\n";

        // Lancement des threads Auth
        std::cout << "[MAIN] Launching Auth IO threads...\n";
        std::vector<std::thread> threads;
        for (int i = 0; i < auth_threads; ++i) {
            threads.emplace_back([&auth_io, i]() {
                std::cout << "[THREAD] Auth thread " << i << " started\n";
                auth_io.run();
                std::cout << "[THREAD] Auth thread " << i << " stopped\n";
            });
        }

        std::cout << "[MAIN] Launching WebRTC IO threads...\n";
for (int i = 0; i < webrtc_threads; ++i) {
    threads.emplace_back([&webrtc_io, i]() {
        std::cout << "[THREAD] WebRTC thread " << i << " started\n";
        webrtc_io.run();
        std::cout << "[THREAD] WebRTC thread " << i << " stopped\n";
    });
}

        std::cout << "[MAIN] Waiting for threads to finish...\n";
        for (auto& t : threads) {
            t.join();
        }

        std::cout << "[MAIN] All threads stopped cleanly\n";

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Server error: " << e.what() << "\n";
        return 1;
    }

    std::cout << "[MAIN] Server shutdown complete\n";
    return 0;
}