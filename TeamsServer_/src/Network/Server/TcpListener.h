#ifndef TCPLISTENER_H
#define TCPLISTENER_H

#include "../../Utils/BoostErrorHandler.h"
#include "../../includes.h"
#include "../Session/AuthSession.h"

/**
 * @class TcpListener
 * @brief Écouteur TCP sécurisé pour accepter les connexions entrantes.
 *
 * Configure un acceptor TCP sur un endpoint donné et utilise SSL pour sécuriser
 * les communications. Chaque connexion acceptée est gérée par une session
 * spécifique.
 */
template <typename SessionType> class TcpListener {
public:
  TcpListener(asio::io_context &io_context, ssl::context &ssl_ctx,
              tcp::endpoint endpoint);

private:
  void do_accept();

  tcp::acceptor acceptor_;
  ssl::context &ssl_ctx_;
  std::shared_ptr<Handler> handler_;
};

#endif