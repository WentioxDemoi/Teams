#include "WebRTCSession.h"
#include "../../Core/Registeries/WebRTCRegistry.h" //Forward declaration

void WebRTCSession::start() {
    auto self = shared_from_this();
    stream_.async_handshake(ssl::stream_base::server,
        [self](boost::system::error_code ec) {
            if (!ec) self->do_read();
            else BoostErrorHandler::log("WebRTCSession", "Handshake", ec);
        });
}


// Dans do_read()
void WebRTCSession::do_read() {
    auto self = shared_from_this();
    stream_.async_read_some(asio::buffer(buffer_),
        [self](boost::system::error_code ec, std::size_t bytes) {
            if (!ec) {
                self->read_buffer_.append(self->buffer_.data(), bytes);
                
                // Découper par \n comme le client Qt
                size_t pos;
                while ((pos = self->read_buffer_.find('\n')) != std::string::npos) {
                    std::string message = self->read_buffer_.substr(0, pos);
                    self->read_buffer_.erase(0, pos + 1);
                    
                    if (!message.empty()) {
                        auto callback = [self](std::string response) {
                            asio::post(self->stream_.get_executor(),
                                [self, response]() { self->handle_response(response); });
                        };
                        self->handler_->handle_type(message, callback, self);
                    }
                }
                self->do_read();
            } else {
                if (!self->uuid_.empty())
                    self->registry_->unregister_session(self->uuid_);
                BoostErrorHandler::log("WebRTCSession", "Read", ec);
            }
        });
}

void WebRTCSession::handle_response(std::string payload) {
    send(payload);
}

void WebRTCSession::send(const std::string& payload) {
    auto self = shared_from_this();
    asio::async_write(stream_, asio::buffer(payload + "\n"),
        [self](boost::system::error_code ec, std::size_t) {
            if (ec) BoostErrorHandler::log("WebRTCSession", "Write", ec);
        });
}