#include "AppCompositionRoot.h"

#include "Services/Auth/AuthService.h"
// #include "Services/User/UserService.h"
#include <iostream>
#include <memory>

AppCompositionRoot::AppCompositionRoot(int auth_threads, int webrtc_threads)
    : auth_threads_(auth_threads),
      webrtc_threads_(webrtc_threads),
      ssl_ctx_(ssl::context::tlsv12_server) {
  initSsl();
  initRegistries();
  initServices();
  initHandlers();
  initListeners();
}

void AppCompositionRoot::initSsl() {
  ssl_ctx_.use_certificate_chain_file("server.crt");
  ssl_ctx_.use_private_key_file("server.key", ssl::context::pem);
}

void AppCompositionRoot::initRegistries() {
  messageSessionRegistry_ = std::make_shared<MessageSessionRegistry>();
  // webRTCRegistry_ = std::make_shared<WebRTCRegistry>();
}

void AppCompositionRoot::initServices() {
  authService_ = std::make_shared<AuthService>(std::make_unique<UserRepository>());
  messageService_ = std::make_unique<MessageService>(std::make_unique<MessageRepository>(),
                                                     messageSessionRegistry_);
  // webRTCService_ = std::make_shared<WebRTCService>();
}

void AppCompositionRoot::initHandlers() {
  authHandler_ = std::make_shared<AuthHandler>(authService_);
  messageHandler_ = std::make_shared<MessageHandler>(std::move(messageService_));
  // webrtcHandler_ = std::make_shared<WebRTCHandler>(std::make_unique<WebRTCService>());
}

void AppCompositionRoot::initListeners() {
  auth_listener_ = std::make_unique<TcpListenerAuth>(
      auth_io_, ssl_ctx_, tcp::endpoint(tcp::v4(), 8080), std::move(authHandler_));

  message_listener_ = std::make_unique<TcpListenerMessage>(
      webrtc_io_, ssl_ctx_, tcp::endpoint(tcp::v4(), 8082), std::move(messageHandler_),
      messageSessionRegistry_, authService_);

  // webrtc_listener_ = std::make_unique<TcpListenerWebRTC>(
  //     webrtc_io_,
  //     ssl_ctx_,
  //     tcp::endpoint(tcp::v4(), 8081)
  // );
}

void AppCompositionRoot::run() {
  std::cout << "[APP] Auth   listener started on port 8080\n";
  std::cout << "[APP] WebRTC listener started on port 8081\n";

  launchThreads();

  for (auto& t : threads_) t.join();

  std::cout << "[APP] All threads stopped cleanly\n";
}

// private

void AppCompositionRoot::launchThreads() {
  for (int i = 0; i < auth_threads_; ++i)
    threads_.emplace_back([this, i]() {
      std::cout << "[THREAD] Auth thread " << i << " started\n";
      auth_io_.run();
      std::cout << "[THREAD] Auth thread " << i << " stopped\n";
    });

  for (int i = 0; i < webrtc_threads_; ++i)
    threads_.emplace_back([this, i]() {
      std::cout << "[THREAD] WebRTC thread " << i << " started\n";
      webrtc_io_.run();
      std::cout << "[THREAD] WebRTC thread " << i << " stopped\n";
    });
}
