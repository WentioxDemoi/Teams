#ifndef MESSAGESESSION_H
#define MESSAGESESSION_H

#include <memory>

#include "../../Handlers/MessageHandler.h"
#include "../../Utils/BoostErrorHandler.h"
#include "../../includes.h"

/**
 * @class MessageSession
 * @brief Représente une session SSL individuelle pour un client TCP.
 *
 * Gère la communication avec un client spécifique sur le port 8082,
 * et délègue le traitement des requêtes à MessageHandler.
 */
class MessageSession : public std::enable_shared_from_this<MessageSession> {
 public:
  MessageSession(tcp::socket socket, ssl::context& ctx) : stream_(std::move(socket), ctx) {
    messageHandler_ =
        std::make_unique<MessageHandler>([this](std::string response) { handle_response(std::move(response)); });
  }

  void start();

 private:
  void do_read();
  void handle_response(std::string payload);

  ssl::stream<tcp::socket> stream_;
  std::array<char, 4096> buffer_;
  std::unique_ptr<MessageHandler> messageHandler_;
};

#endif