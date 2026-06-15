#ifndef TCPLISTENERMESSAGE_H
#define TCPLISTENERMESSAGE_H

#include "../../Utils/BoostErrorHandler.h"
#include "../../includes.h"
#include "../Session/MessageSession.h"
#include <memory>
#include <unordered_map>

class TcpListenerMessage {
public:
  TcpListenerMessage(asio::io_context &io_context, ssl::context &ssl_ctx,
                     tcp::endpoint endpoint);

  void send_to(const std::string& user_uuid, const std::string& payload);
  void register_session(const std::string& user_uuid, std::shared_ptr<MessageSession> session);
  void unregister_session(const std::string& user_uuid);

private:
  void do_accept();

  tcp::acceptor acceptor_;
  ssl::context &ssl_ctx_;
  std::unordered_map<std::string, std::shared_ptr<MessageSession>> sessions_;
};

#endif