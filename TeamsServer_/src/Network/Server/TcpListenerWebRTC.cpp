#include "TcpListenerWebRTC.h"
// #include "../../Core/Registeries/WebRTCRegistry.h"


TcpListenerWebRTC::TcpListenerWebRTC(asio::io_context &io_context,
                                     ssl::context &ssl_ctx,
                                     tcp::endpoint endpoint)
    : acceptor_(io_context, endpoint), ssl_ctx_(ssl_ctx) {
    registry_ = std::make_shared<WebRTCRegistry>();
    handler_  = std::make_shared<WebRTCHandler>(registry_);
    do_accept();
}

void TcpListenerWebRTC::do_accept() {
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            auto session = std::make_shared<WebRTCSession>(
                std::move(socket), ssl_ctx_, handler_, registry_
            );
            session->start();
        } else {
            BoostErrorHandler::log("TCPListenerWebRTC", "Accept", ec);
        }
        do_accept();
    });
}