#ifndef APPCOMPOSITIONROOT_H
#define APPCOMPOSITIONROOT_H

#include <future>
#include <memory>
#include <thread>
#include <vector>

#include "Core/Registeries/MessageSessionRegistry.h"
#include "Core/Registeries/WebRTCRegistry.h"
#include "Handlers/MessageHandler.h"
#include "Infrastructure/Database/DatabaseManager.h"
#include "Network/Server/TcpListenerAuth.h"
#include "Network/Server/TcpListenerMessage.h"
#include "Network/Server/TcpListenerWebRTC.h"
#include "includes.h"

/**
 * @class AppCompositionRoot
 * @brief Point d'entrée unique de l'application — câble et démarre tous les
 *        services et listeners.
 *
 * Instancie les singletons (AuthService, UserService…) une seule fois et les
 * injecte par référence dans les listeners. Responsable du cycle de vie des
 * io_context et des threads d'exécution.
 */
class AppCompositionRoot {
 public:
  AppCompositionRoot(int auth_threads, int webrtc_threads);

  void run();

 private:
  void initSsl();
  void initServices();
  void initRegistries();
  void initListeners();
  void launchThreads();
  void initHandlers();

  const int auth_threads_;
  const int webrtc_threads_;

  asio::io_context auth_io_;
  asio::io_context webrtc_io_;
  ssl::context ssl_ctx_;

  std::shared_ptr<AuthService> authService_;
  std::unique_ptr<MessageService> messageService_;
    // std::shared_ptr<WebRTCService> webRTCService_;


  std::shared_ptr<MessageSessionRegistry> messageSessionRegistry_;
  // std::shared_ptr<WebRTCRegistry> webRTCRegistry_; A venir pour gérer les sessions WebRTC

  std::unique_ptr<TcpListenerAuth> auth_listener_;
  std::unique_ptr<TcpListenerMessage> message_listener_;
  std::unique_ptr<TcpListenerWebRTC> webrtc_listener_;

  std::shared_ptr<AuthHandler> authHandler_;
  std::shared_ptr<MessageHandler> messageHandler_;
  std::shared_ptr<WebRTCHandler> webrtcHandler_;

  std::vector<std::thread> threads_;
};

#endif