#ifndef WEBRTCHANDLER_H
#define WEBRTCHANDLER_H

#include "../Core/Repositories/UserRepository.h"
#include "../Core/Services/WebRTCService.h"
#include "../Utils/ResponseFormater.h"
#include "../includes.h"
#include "../Utils/PacketHelper.h"
#include "../Core/Services/WebRTCService.h"


using ResponseCallback = std::function<void(std::string)>;
class WebRTCSession;

/**
 * @class WebRTCHandler
 * @brief 
 */
class WebRTCHandler {
public:
    // shared_ptr<WebRTCSession> en forward declaration pour éviter la dépendance circulaire
    void handle_type(std::string uuid, std::string payload, ResponseCallback respond);

    WebRTCHandler(std::unique_ptr<WebRTCService> webRTCService)
        : worker_pool_(Config::instance().worker_pool_size()), webRTCService_(std::move(webRTCService)) {}

private:

    void handle_call_request(const std::string &uuid, std::string payload, ResponseCallback respond);
    void handle_call_accept(const std::string &uuid, std::string payload, ResponseCallback respond);
    void handle_call_reject(const std::string &uuid, std::string payload, ResponseCallback respond);
    void handle_call_cancel(const std::string &uuid, std::string payload, ResponseCallback respond);
    void handle_call_hangup(const std::string &uuid, std::string payload, ResponseCallback respond);
    void handle_camera_enabled_change(const std::string &uuid, std::string payload, ResponseCallback respond);
    void handle_signaling_relay(const std::string &uuid, std::string payload, ResponseCallback respond);

    bool relay_to_target(const std::string &senderUuid, const std::string &targetUuid, std::string payload);


    asio::thread_pool worker_pool_;

protected:
    std::unique_ptr<WebRTCService> webRTCService_;
};

#endif
