#ifndef WEBRTCHANDLER_H
#define WEBRTCHANDLER_H

#include "../Core/Repositories/UserRepository.h"
#include "../Core/Services/WebRTCService.h"
#include "../Utils/ResponseFormater.h"
#include "../includes.h"
#include "HandlerTools.h"
#include "../Core/Services/WebRTCService.h"
#include "../Core/Registeries/WebRTCRegistry.h"


using ResponseCallback = std::function<void(std::string)>;
class WebRTCSession;

/**
 * @class WebRTCHandler
 * @brief 
 */
class WebRTCHandler {
public:
    // shared_ptr<WebRTCSession> en forward declaration pour éviter la dépendance circulaire
    void handle_type(std::string payload, ResponseCallback respond,
                     std::shared_ptr<WebRTCSession> session);
    void handle_register(std::string payload, ResponseCallback respond,
                         std::shared_ptr<WebRTCSession> session);
    void handle_send(std::string payload, ResponseCallback respond);

    WebRTCHandler(std::shared_ptr<WebRTCRegistry> registry)
        : worker_pool_(Config::instance().worker_pool_size()), registry_(registry) {
        WebRTCService_ = std::make_unique<WebRTCService>(std::make_unique<UserRepository>());
    }

private:
    asio::thread_pool worker_pool_;
    std::shared_ptr<WebRTCRegistry> registry_;

protected:
    std::unique_ptr<WebRTCService> WebRTCService_;
};

#endif