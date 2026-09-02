#ifndef TCPLISTENERCONTACT_H
#define TCPLISTENERCONTACT_H

#include <memory>
#include <unordered_map>

#include "../../Core/Registeries/ContactSessionRegistry.h"
#include "../../Core/Services/AuthService.h"
#include "../../Handlers/ContactHandler.h"
#include "../../includes.h"
#include "../Session/ContactSession.h"

class TcpListenerContact {
 public:
  TcpListenerContact(asio::io_context& io_context, ssl::context& ssl_ctx, tcp::endpoint endpoint,
                     std::shared_ptr<ContactHandler> contactHandler,
                     std::shared_ptr<IContactSessionRegistry> contactSessionRegistry,
                     std::shared_ptr<IAuthService> authService);

 private:
  void do_accept();

  std::shared_ptr<ContactHandler> contactHandler_;
  std::shared_ptr<IContactSessionRegistry> contactSessionRegistry_;
  std::shared_ptr<IAuthService> authService_;

  tcp::acceptor acceptor_;
  ssl::context& ssl_ctx_;

  std::unordered_map<std::string, std::shared_ptr<ContactSession>> sessions_;
};

#endif