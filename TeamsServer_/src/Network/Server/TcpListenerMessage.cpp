#include "TcpListenerMessage.h"
#include "../../Utils/BoostErrorHandler.h"

TcpListenerMessage::TcpListenerMessage(
    asio::io_context& io_context, ssl::context& ssl_ctx, tcp::endpoint endpoint,
    std::shared_ptr<MessageHandler> messageHandler,
    std::shared_ptr<MessageSessionRegistry> messageSessionRegistry,
    std::shared_ptr<AuthService> authService)
    : acceptor_(io_context, endpoint),
      ssl_ctx_(ssl_ctx),
      messageHandler_(messageHandler),
      messageSessionRegistry_(messageSessionRegistry),
      authService_(authService) {
  do_accept();
}

void TcpListenerMessage::do_accept() {
  acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
    if (!ec) {
      std::cout << "[MessageServer] New connection\n";

      auto session = std::make_shared<MessageSession>(
          std::move(socket), ssl_ctx_, messageHandler_, messageSessionRegistry_, authService_);
      session->start();
    } else {
      BoostErrorHandler::log("TcpListenerMessage", "Accept", ec);
    }

    do_accept();
  });
}