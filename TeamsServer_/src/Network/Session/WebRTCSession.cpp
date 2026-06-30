#include "WebRTCSession.h"
#include "../../Core/Registeries/WebRTCRegistry.h"
#include "../../Utils/PacketHelper.h"
#include "../../Core/Services/AuthService.h"

void WebRTCSession::start() {
    auto self = shared_from_this();
    stream_.async_handshake(ssl::stream_base::server,
        [self](boost::system::error_code ec) {
            if (!ec) {
                self->do_read();
            } else {
                BoostErrorHandler::log("WebRTCSession", "Handshake", ec);
            }
        });
}

void WebRTCSession::do_read() {
    auto self = shared_from_this();
    stream_.async_read_some(asio::buffer(buffer_), [self](boost::system::error_code ec,
                                                            std::size_t bytes) {
        if (!ec) {
            std::string payload(self->buffer_.data(), bytes);
            // std::cout << "WebRTC: " << payload << std::endl;

            if (self->isFirstMessage_) {
                auto token = PacketHelper::extractValue(payload, "token");
                auto user = self->authService_->validateToken(token);
                if (!user) {
                    std::cerr << "[WebRTCSession] Invalid token: " << token << "\n";
                    self->send(R"({"type":"error","error":"Invalid token"})");
                    self->do_read();
                    return;
                }
                self->isFirstMessage_ = false;
                self->user_uuid_ = user->uuid;
                self->webRTCSessionRegistry_->register_session(self->user_uuid_, self);
                std::cout << "[WebRTCSession] User authenticated with UUID: "
                          << self->user_uuid_ << "\n";
                self->do_read();
                return;
            }

            auto callback = [self](std::string response) {
                asio::post(self->stream_.get_executor(),
                    [self, response]() { self->send(response); });
            };
            self->webRTCHandler_->handle_type(self->user_uuid_, payload, callback);
            self->do_read();
        } else {
            if (ec == asio::error::eof || ec == boost::asio::ssl::error::stream_truncated ||
                ec == asio::error::connection_reset) {
                std::cout << "[WebRTCSession] Client disconnected" << std::endl;
                if (!self->user_uuid_.empty())
                    self->webRTCSessionRegistry_->unregister_session(self->user_uuid_);
                return;
            }

            std::cerr << "[WebRTCSession] Read error: " << ec.message() << std::endl;
        }
    });
}

void WebRTCSession::send(const std::string& payload) {
    auto self = shared_from_this();

    std::cout << "[WebRTCSession] Sending message to " << self->user_uuid_ << ": " << payload << std::endl;
    asio::async_write(stream_, asio::buffer(payload + "\n"),
        [self](boost::system::error_code ec, std::size_t) {
            if (ec) {
                BoostErrorHandler::log("WebRTCSession", "Write", ec);
            }
        });
}