#include "TcpListenerMessage.h"

TcpListenerMessage::TcpListenerMessage(asio::io_context &io_context,
                                       ssl::context &ssl_ctx,
                                       tcp::endpoint endpoint)
    : acceptor_(io_context, endpoint), ssl_ctx_(ssl_ctx) {
  do_accept();
}

void TcpListenerMessage::do_accept() {
  acceptor_.async_accept([this](boost::system::error_code ec,
                                tcp::socket socket) {
    if (!ec) {
      std::cout << "[MessageServer] New connection\n";

      auto session = std::make_shared<MessageSession>(std::move(socket), ssl_ctx_);
      session->start();
    } else {
      BoostErrorHandler::log("TcpListenerMessage", "Accept", ec);
    }

    do_accept();
  });
}

void TcpListenerMessage::send_to(const std::string& user_uuid, const std::string& payload) {
  auto it = sessions_.find(user_uuid);
  if (it != sessions_.end()) {
    it->second->send(payload);
  }
}

void TcpListenerMessage::register_session(const std::string& user_uuid,
                                          std::shared_ptr<MessageSession> session) {
  sessions_[user_uuid] = session;
}

void TcpListenerMessage::unregister_session(const std::string& user_uuid) {
  sessions_.erase(user_uuid);
}