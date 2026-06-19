#include "TcpListenerContact.h"
#include "../../Utils/BoostErrorHandler.h"

TcpListenerContact::TcpListenerContact(
    asio::io_context& io_context,
    ssl::context& ssl_ctx,
    tcp::endpoint endpoint,
    std::shared_ptr<ContactHandler> contactHandler,
    // std::shared_ptr<ContactSessionRegistry> contactSessionRegistry,
    std::shared_ptr<AuthService> authService)
    : acceptor_(io_context, endpoint),
      ssl_ctx_(ssl_ctx),
      contactHandler_(contactHandler),
      // contactSessionRegistry_(contactSessionRegistry),
      authService_(authService) {
  do_accept();
}

void TcpListenerContact::do_accept() {
  acceptor_.async_accept(
      [this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
          std::cout << "[ContactServer] New connection\n";

          auto session = std::make_shared<ContactSession>(
              std::move(socket),
              ssl_ctx_,
              contactHandler_,
              // contactSessionRegistry_,
              authService_);

          session->start();
        } else {
          BoostErrorHandler::log("TcpListenerContact", "Accept", ec);
        }

        do_accept();
      });
}