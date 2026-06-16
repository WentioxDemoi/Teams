#ifndef MESSAGESESSION_H
#define MESSAGESESSION_H

#include <functional>
#include <memory>

#include "../../Handlers/MessageHandler.h"
#include "../../Utils/BoostErrorHandler.h"
#include "../../includes.h"

using SendToCallback = std::function<void(const std::string&, const std::string&)>;

/**
 * @class MessageSession
 * @brief Représente une session SSL individuelle pour un client TCP.
 *
 * Gère la communication avec un client spécifique sur le port 8082,
 * et délègue le traitement des requêtes à MessageHandler.
 */
class MessageSession : public std::enable_shared_from_this<MessageSession> {
 public:
  MessageSession(tcp::socket socket, ssl::context& ctx, std::shared_ptr<MessageHandler> messageHandler, SendToCallback send_to_callback)
      : stream_(std::move(socket), ctx), messageHandler_(messageHandler), send_to_callback_(send_to_callback) {
  }

  void start();
  void send(const std::string& payload);

 private:
  void do_read();

  ssl::stream<tcp::socket> stream_;
  std::array<char, 4096> buffer_;
  std::shared_ptr<MessageHandler> messageHandler_;

  SendToCallback send_to_callback_;
};

#endif