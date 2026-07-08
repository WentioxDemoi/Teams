#ifndef MESSAGESESSION_H
#define MESSAGESESSION_H

#include <functional>
#include <memory>

#include "../../Core/Registeries/MessageSessionRegistry.h"
#include "../../Handlers/MessageHandler.h"
#include "../../Utils/BoostErrorHandler.h"
#include "../../includes.h"
#include "../../Core/Services/AuthService.h"


/**
 * @class MessageSession
 * @brief Représente une session SSL individuelle pour un client TCP.
 *
 * Gère la communication avec un client spécifique sur le port 8082,
 * et délègue le traitement des requêtes à MessageHandler.
 */
class MessageSession : public std::enable_shared_from_this<MessageSession> {
 public:
  MessageSession(tcp::socket socket, ssl::context& ctx,
                 std::shared_ptr<MessageHandler> messageHandler,
                 std::shared_ptr<MessageSessionRegistry> messageSessionRegistry,
                 std::shared_ptr<AuthService> authService)
      : stream_(std::move(socket), ctx),
        messageHandler_(messageHandler),
        messageSessionRegistry_(messageSessionRegistry),
        authService_(authService) {}

  void start();
  void send(const std::string& payload);

 private:
  void do_read();

  ssl::stream<tcp::socket> stream_;
  std::array<char, 4096> buffer_;
  std::shared_ptr<MessageHandler> messageHandler_;
  std::shared_ptr<MessageSessionRegistry> messageSessionRegistry_;
  std::shared_ptr<AuthService> authService_;
  std::string user_uuid_;
  bool isFirstMessage_ = true;
};

#endif