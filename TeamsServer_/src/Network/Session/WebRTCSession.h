#ifndef WEBRTCSESSION_H
#define WEBRTCSESSION_H

#include "../../Handlers/WebRTCHandler.h"
#include "../../Utils/BoostErrorHandler.h"
#include "../../includes.h"

class WebRTCRegistry;
class AuthService;

class WebRTCSession : public std::enable_shared_from_this<WebRTCSession> {
public:
  WebRTCSession(tcp::socket socket, ssl::context &ctx, std::shared_ptr<WebRTCHandler> webRTCHandler,
                std::shared_ptr<WebRTCRegistry> webRTCSessionRegistry, std::shared_ptr<AuthService> authService)
      : stream_(std::move(socket), ctx), webRTCHandler_(webRTCHandler), webRTCSessionRegistry_(webRTCSessionRegistry),
        authService_(authService) {}

  void start();
  void send(const std::string &payload); // Appelé par le registry pour router un message

  ssl::stream<tcp::socket> stream_;

private:
  void do_read();
  void handle_response(std::string payload);
  std::string read_buffer_;

  std::array<char, 4096> buffer_;
  std::shared_ptr<WebRTCHandler> webRTCHandler_;
  std::shared_ptr<WebRTCRegistry> webRTCSessionRegistry_;
  std::shared_ptr<AuthService> authService_;
  bool isFirstMessage_ = true;
  std::string user_uuid_;
};
#endif