#ifndef TCPLISTENERAUTH_H
#define TCPLISTENERAUTH_H

#include <memory>
#include "../../Handlers/AuthHandler.h"
/**
 * @class TcpListenerAuth
 * @brief Écouteur TCP sécurisé pour accepter les connexions entrantes.
 *
 * Configure un acceptor TCP sur un endpoint donné et utilise SSL pour sécuriser
 * les communications. Chaque connexion acceptée est gérée par une session
 * spécifique.
 */
class TcpListenerAuth {
public:
  TcpListenerAuth(asio::io_context &io_context, ssl::context &ssl_ctx,
              tcp::endpoint endpoint, std::shared_ptr<AuthHandler> authHandler);

private:
  void do_accept();

  std::shared_ptr<AuthHandler> authHandler_;
  tcp::acceptor acceptor_;
  ssl::context &ssl_ctx_;
};

#endif