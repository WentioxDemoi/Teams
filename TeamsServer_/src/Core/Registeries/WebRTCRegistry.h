
#ifndef WEBRTCREGISTERY_H
#define WEBRTCREGISTERY_H

#include "../Repositories/UserRepository.h"
#include "../Services/AuthService.h"
#include "../../Utils/ResponseFormater.h"
#include "../../includes.h"

class WebRTCSession;

class WebRTCRegistry {
public:
    void register_session(const std::string& uuid,
                          std::shared_ptr<WebRTCSession> session);

    std::shared_ptr<WebRTCSession> find(const std::string& uuid);

    void unregister_session(const std::string& uuid);

private:
    std::unordered_map<std::string,
        std::weak_ptr<WebRTCSession>> sessions_;
    std::mutex mutex_;
};

#endif