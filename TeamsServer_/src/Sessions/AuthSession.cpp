#include "AuthSession.h"

void AuthSession::start() {
  auto self = shared_from_this();
  stream_.async_handshake(
      ssl::stream_base::server, [this, self](boost::system::error_code ec) {
        if (!ec) {
          do_read();
        } else {
          handle_error("AuthSession Handshake : ", ec);
        }
      });
}

void AuthSession::do_read() {
  auto self = shared_from_this();
  stream_.async_read_some(
      asio::buffer(buffer_),
      [this, self](boost::system::error_code ec, std::size_t bytes) {
        if (!ec) {
          do_read();
        } else {
          handle_error("AuthSession Read : ", ec);
        }
      });
}