#ifndef TCPLISTENERCONTACT_H
#define TCPLISTENERCONTACT_H

#include <memory>
#include <unordered_map>

// #include "../../Core/Registeries/ContactSessionRegistry.h"
#include "../../Handlers/ContactHandler.h"
#include "../../includes.h"
#include "../Session/ContactSession.h"
#include "../../Core/Services/AuthService.h"

class TcpListenerContact {
 public:
  TcpListenerContact(
      asio::io_context& io_context,
      ssl::context& ssl_ctx,
      tcp::endpoint endpoint,
      std::shared_ptr<ContactHandler> contactHandler,
      // std::shared_ptr<ContactSessionRegistry> contactSessionRegistry,
      std::shared_ptr<AuthService> authService);

 private:
  void do_accept();

  std::shared_ptr<ContactHandler> contactHandler_;
  // std::shared_ptr<ContactSessionRegistry> contactSessionRegistry_;
  std::shared_ptr<AuthService> authService_;

  tcp::acceptor acceptor_;
  ssl::context& ssl_ctx_;

  // std::unordered_map<std::string, std::shared_ptr<ContactSession>> sessions_;
};

#endif