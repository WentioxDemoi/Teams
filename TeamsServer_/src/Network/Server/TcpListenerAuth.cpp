#include "TcpListenerAuth.h"


TcpListenerAuth::TcpListenerAuth(asio::io_context &io_context,
                                      ssl::context &ssl_ctx,
                                      tcp::endpoint endpoint, std::shared_ptr<AuthHandler> authHandler)
    : acceptor_(io_context, endpoint), ssl_ctx_(ssl_ctx), authHandler_(authHandler) {
  do_accept();
}

void TcpListenerAuth::do_accept() {
  acceptor_.async_accept([this](boost::system::error_code ec,
                                tcp::socket socket) {
    if (!ec) {
      std::cout << "[Server] New connection\n";

      auto session =
          std::make_shared<AuthSession>(std::move(socket), ssl_ctx_, authHandler_);

      session->start();
    } else {
      BoostErrorHandler::log("TCPListener", "Accept", ec);
    }

    do_accept();
  });
}
