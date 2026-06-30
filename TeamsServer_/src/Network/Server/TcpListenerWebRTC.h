#ifndef TCPLISTENERWEBRTC_H
#define TCPLISTENERWEBRTC_H

#include "../../Utils/BoostErrorHandler.h"
#include "../../includes.h"
#include "../Session/WebRTCSession.h"
#include "../../Core/Registeries/WebRTCRegistry.h"

// class WebRTCRegistry;
/**
 * @class TcpListenerWebRTC
 * @brief Écouteur TCP sécurisé pour accepter les connexions entrantes.
 *
 * Configure un acceptor TCP sur un endpoint donné et utilise SSL pour sécuriser
 * les communications. Chaque connexion acceptée est gérée par une session
 * spécifique.
 */
class TcpListenerWebRTC {
public:
  TcpListenerWebRTC(asio::io_context &io_context, ssl::context &ssl_ctx,
              tcp::endpoint endpoint, std::shared_ptr<WebRTCRegistry> webRTCRegistry, std::shared_ptr<WebRTCHandler> webrtcHandler, std::shared_ptr<AuthService> authService);

private:
  void do_accept();

  tcp::acceptor acceptor_;
  ssl::context &ssl_ctx_;
  std::shared_ptr<AuthService> authService_;
  std::shared_ptr<WebRTCHandler> webRTCHandler_;
  std::shared_ptr<WebRTCRegistry> webRTCRegistry_;
};

#endif