#include "ErrorManager.h"

void ErrorManager::handle_error(std::string_view where, const boost::system::error_code& ec) {
    std::cerr << "[AuthSession][" << where << "] Error: "
              << ec.message() << " (" << ec.value() << ")\n";

    // Quelques erreurs typiques qu’on traite explicitement :
    if (ec == asio::error::eof) {
        std::cerr << "Client disconnected cleanly\n";
    }
    else if (ec == asio::ssl::error::stream_truncated) {
        std::cerr << "SSL shutdown incorrect (client closed sans handshake)\n";
    }
    else if (ec == asio::error::connection_reset) {
        std::cerr << "Connection reset\n";
    }
}