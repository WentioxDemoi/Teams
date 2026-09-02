#ifndef APPCOMPOSITIONROOT_H
#define APPCOMPOSITIONROOT_H

#include <future>
#include <memory>
#include <thread>
#include <vector>

#include "Core/Services/AuthService.h"
#include "Core/Registeries/MessageSessionRegistry.h"
#include "Core/Registeries/IMessageSessionRegistry.h"
#include "Core/Registeries/WebRTCRegistry.h"
#include "Handlers/MessageHandler.h"
#include "Infrastructure/DatabaseManager.h"
#include "Network/Server/TcpListenerAuth.h"
#include "Network/Server/TcpListenerMessage.h"
#include "Network/Server/TcpListenerWebRTC.h"
#include "Repositories/ContactRepository.h"
#include "Repositories/IMessageRepository.h"
#include "Repositories/MessageRepository.h"
#include "Server/TcpListenerContact.h"
#include "Services/ContactService.h"

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
  AppCompositionRoot(int auth_threads, int message_threads, int contact_threads, int webrtc_threads);

  void run();

 private:
  void initSsl();
  void initRepositories();
  void initServices();
  void initRegistries();
  void initListeners();
  void launchThreads();
  void initHandlers();

  const int auth_threads_;
  const int message_threads_;
  const int contact_threads_;
  const int webrtc_threads_;

  asio::io_context auth_io_;
  asio::io_context message_io_;
  asio::io_context contact_io_;
  asio::io_context webrtc_io_;
  ssl::context ssl_ctx_;

  std::shared_ptr<IAuthService> authService_;
  std::unique_ptr<IMessageService> messageService_;
  std::unique_ptr<IContactService> contactService_;
  std::unique_ptr<IWebRTCService> webRTCService_;


  std::shared_ptr<IMessageSessionRegistry> messageSessionRegistry_;
  std::shared_ptr<IContactSessionRegistry> contactSessionRegistry_;
  std::shared_ptr<IWebRTCRegistry> webRTCRegistry_;

  std::unique_ptr<TcpListenerAuth> auth_listener_;
  std::unique_ptr<TcpListenerMessage> message_listener_;
  std::unique_ptr<TcpListenerContact> contact_listener_;
  std::unique_ptr<TcpListenerWebRTC> webrtc_listener_;

  std::shared_ptr<AuthHandler> authHandler_;
  std::shared_ptr<MessageHandler> messageHandler_;
  std::shared_ptr<ContactHandler> contactHandler_;
  std::shared_ptr<WebRTCHandler> webrtcHandler_;

  std::shared_ptr<IUserRepository> userRepository_;
  std::unique_ptr<IContactRepository> contactRepository_;
  std::unique_ptr<IMessageRepository> messageRepository_;

  std::vector<std::thread> threads_;
};

#endif