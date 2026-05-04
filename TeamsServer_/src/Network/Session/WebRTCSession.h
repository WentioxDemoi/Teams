#ifndef WEBRTCSESSION_H
#define WEBRTCSESSION_H

#include "../../Handlers/WebRTCHandler.h"
#include "../../Utils/BoostErrorHandler.h"
#include "../../includes.h"

class WebRTCRegistry;

class WebRTCSession : public std::enable_shared_from_this<WebRTCSession> {
public:
    WebRTCSession(tcp::socket socket, ssl::context &ctx,
                  std::shared_ptr<WebRTCHandler> handler,
                  std::shared_ptr<WebRTCRegistry> registry)
        : stream_(std::move(socket), ctx), handler_(handler), registry_(registry) {}

    void start();
    void send(const std::string& payload); // Appelé par le registry pour router un message

    const std::string& uuid() const { return uuid_; }
    void set_uuid(std::string uuid) { uuid_ = uuid; } 
    ssl::stream<tcp::socket> stream_;

private:
    void do_read();
    void handle_response(std::string payload);
    std::string read_buffer_;

    
    std::array<char, 4096> buffer_;
    std::shared_ptr<WebRTCHandler> handler_;
    std::shared_ptr<WebRTCRegistry> registry_;
    std::string uuid_; // Vide tant que non enregistré
};
#endif