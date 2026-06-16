#include "MessageSession.h"

void MessageSession::start() {
  auto self = shared_from_this();
  stream_.async_handshake(ssl::stream_base::server, [self](boost::system::error_code ec) {
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
      asio::buffer(buffer_), [self](boost::system::error_code ec, std::size_t bytes) {
        if (!ec) {
          std::string payload(self->buffer_.data(), bytes);
          std::cout << "Message: " << payload << std::endl;

          if (self->isFirstMessage_) {
            auto token = HandlerTools::extractValue(payload, "token");
            auto user = self->authService_->validateToken(token);
            if (!user) {
              std::cerr << "[MessageSession] Invalid token: " << token << "\n";
              self->send(R"({"type":"error","error":"Invalid token"})");
              self->do_read();
              return;
            }
            self->isFirstMessage_ = false;
            self->user_uuid_ = user->uuid;
            self->messageSessionRegistry_->registerMessageSession(self->user_uuid_, self);
            self->do_read();
            return;
          }

          auto callback = [self](const std::string& response) {
            asio::post(self->stream_.get_executor(), [self, response]() { self->send(response); });
          };
          self->messageHandler_->handle_type(self->user_uuid_, payload, callback);
          self->do_read();
        } else {
          if (!self->user_uuid_.empty())
            self->messageSessionRegistry_->unregisterMessageSession(self->user_uuid_);
          BoostErrorHandler::log("MessageSession", "Read", ec);
        }
      });
}

void MessageSession::send(const std::string& payload) {
  auto self = shared_from_this();
  asio::async_write(stream_, asio::buffer(payload + "\n"),
                    [self](boost::system::error_code ec, std::size_t) {
                      if (ec) {
                        BoostErrorHandler::log("MessageSession", "Write", ec);
                      }
                    });
}