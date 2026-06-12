#include "MessageSession.h"

void MessageSession::start() {
  auto self = shared_from_this();
  stream_.async_handshake(
      ssl::stream_base::server, [self](boost::system::error_code ec) {
        if (!ec) {
          self->do_read();
        } else {
          BoostErrorHandler::log("MessageSession", "Handshake", ec);
        }
      });
}

void MessageSession::do_read() {
  auto self = shared_from_this();
  stream_.async_read_some(
      asio::buffer(buffer_),
      [self](boost::system::error_code ec, std::size_t bytes) {
        if (!ec) {
          std::string payload(self->buffer_.data(), bytes);
          std::cout << "Message: " << payload << std::endl;
          auto callback = [self](std::string response) {
            asio::post(self->stream_.get_executor(),
                       [self, response]() { self->handle_response(response); });
          };
          self->messageHandler_->handle_type(payload, callback);
          self->do_read();
        } else {
          BoostErrorHandler::log("MessageSession", "Read", ec);
        }
      });
}

void MessageSession::handle_response(std::string payload) {
  auto self = shared_from_this();
  asio::async_write(stream_, asio::buffer(payload + "\n"),
                    [self](boost::system::error_code ec, std::size_t) {
                      if (ec) {
                        BoostErrorHandler::log("MessageSession", "Write", ec);
                      }
                    });
}