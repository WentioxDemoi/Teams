#include "TcpListener.h"

template <typename SessionType>
TcpListener<SessionType>::TcpListener(asio::io_context &io_context,
                                      ssl::context &ssl_ctx,
                                      tcp::endpoint endpoint)
    : acceptor_(io_context, endpoint), ssl_ctx_(ssl_ctx) {
  handler_ = std::make_shared<Handler>();
  do_accept();
}

template <typename SessionType> void TcpListener<SessionType>::do_accept() {
  acceptor_.async_accept([this](boost::system::error_code ec,
                                tcp::socket socket) {
    if (!ec) {
      std::cout << "[Server] New connection\n";

      auto session =
          std::make_shared<SessionType>(std::move(socket), ssl_ctx_, handler_);

      session->start();
    } else {
      BoostErrorHandler::log("TCPListener", "Accept", ec);
    }

    do_accept();
  });
}
// Instanciation explicite du template pour la compilation
template class TcpListener<AuthSession>;