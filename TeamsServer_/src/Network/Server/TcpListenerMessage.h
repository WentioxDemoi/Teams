#ifndef TCPLISTENERMESSAGE_H
#define TCPLISTENERMESSAGE_H

#include <memory>
#include <unordered_map>

#include "../../Core/Registeries/MessageSessionRegistry.h"
#include "../../Handlers/AuthHandler.h"
#include "../../Handlers/MessageHandler.h"
#include "../../Utils/BoostErrorHandler.h"
#include "../../includes.h"
#include "../Session/MessageSession.h"
#include "Auth/AuthService.h"

class TcpListenerMessage {
 public:
  TcpListenerMessage(asio::io_context& io_context, ssl::context& ssl_ctx, tcp::endpoint endpoint,
                     std::shared_ptr<MessageHandler> messageHandler,
                     std::shared_ptr<MessageSessionRegistry> messageSessionRegistry,
                     std::shared_ptr<AuthService> authService);

 private:
  void do_accept();

  std::shared_ptr<MessageHandler> messageHandler_;
  std::shared_ptr<MessageSessionRegistry> messageSessionRegistry_;
  std::shared_ptr<AuthService> authService_;
  tcp::acceptor acceptor_;
  ssl::context& ssl_ctx_;
  std::unordered_map<std::string, std::shared_ptr<MessageSession>> sessions_;
};

#endif