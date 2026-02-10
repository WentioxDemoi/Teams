#ifndef BOOSTERRORHANDLER_H
#define BOOSTERRORHANDLER_H

#include "../includes.h"

/**
 * @class BoostErrorHandler
 * @brief Gestionnaire statique des erreurs Boost.Asio et SSL.
 *
 * Fournit des outils pour journaliser les erreurs réseau et SSL de manière
 * détaillée, en identifiant le composant et l'endroit où l'erreur s'est
 * produite.
 */
class BoostErrorHandler {
public:
  static void log(std::string_view component, std::string_view where,
                  const boost::system::error_code &ec) {
    std::cerr << "[" << component << "][" << where
              << "] Error: " << ec.message() << " (" << ec.value() << ")\n";

    if (ec == asio::error::eof) {
      std::cerr << "Client disconnected cleanly\n";
    } else if (ec == asio::ssl::error::stream_truncated) {
      std::cerr << "SSL shutdown incorrect\n";
    } else if (ec == asio::error::connection_reset) {
      std::cerr << "Connection reset\n";
    }
  }

  BoostErrorHandler() = delete;
};

#endif