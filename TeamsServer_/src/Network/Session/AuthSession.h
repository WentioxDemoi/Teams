#ifndef AUTHSESSION_H
#define AUTHSESSION_H

#include <memory>

#include "../../Handlers/AuthHandler.h"
#include "../../Utils/BoostErrorHandler.h"
#include "../../includes.h"

/**
 * @class AuthSession
 * @brief Représente une session SSL individuelle pour un client TCP.
 *
 * Gère la communication avec un client spécifique, lit les données entrantes,
 * et délègue le traitement des requêtes au Handler central.
 */
class AuthSession : public std::enable_shared_from_this<AuthSession> {
 public:
  AuthSession(tcp::socket socket, ssl::context& ctx) : stream_(std::move(socket), ctx) {
    authHandler_ =
        std::make_unique<AuthHandler>([this](std::string response) { handle_response(std::move(response)); });
  }

  void start();
  

 private:
  void do_read();
  void handle_response(std::string payload);

  ssl::stream<tcp::socket> stream_;
  std::array<char, 4096> buffer_;
  std::unique_ptr<AuthHandler> authHandler_;
};

#endif