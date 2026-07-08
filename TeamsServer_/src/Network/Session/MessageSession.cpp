#include "MessageSession.h"

#include "../../Utils/PacketHelper.h"

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
  // Attention async read some à une limite, côté client on peut envoyer le message en plusieurs paquets alors qu'ici on les prends 1 par 1
  stream_.async_read_some(asio::buffer(buffer_), [self](boost::system::error_code ec,
                                                        std::size_t bytes) {
    if (!ec) {
      std::string payload(self->buffer_.data(), bytes);
      // std::cout << "Message: " << payload << std::endl;

      if (self->isFirstMessage_) {
        auto token = PacketHelper::extractValue(payload, "token");
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
        std::cout << "[MessageSession] User authenticated with UUID: " << self->user_uuid_ << "\n";
        self->do_read();
        return;
      }

      auto callback = [self](const std::string& response) {
        asio::post(self->stream_.get_executor(), [self, response]() { self->send(response); });
      };
      self->messageHandler_->handle_type(self->user_uuid_, payload, callback);
      self->do_read();
    } else {
      if (ec == asio::error::eof || ec == boost::asio::ssl::error::stream_truncated ||
          ec == asio::error::connection_reset) {
        std::cout << "[MessageSession] Client disconnected" << std::endl;
        self->messageSessionRegistry_->unregisterMessageSession(self->user_uuid_);
        return;
      }

      std::cerr << "[MessageSession] Read error: " << ec.message() << std::endl;
    }
  });
}

// Plus tard il faudra utiliser une COROUTINE avec timeout pour gérer les envois de messages
void MessageSession::send(const std::string& payload) {
  auto self = shared_from_this();

  std::cout << "[MessageSession] Sending message to " << self->user_uuid_ << ": " << payload << std::endl;
  asio::async_write(stream_, asio::buffer(payload + "\n"),
                    [self](boost::system::error_code ec, std::size_t) {
                      if (ec) {
                        BoostErrorHandler::log("MessageSession", "Write", ec);
                      }
                    });
}