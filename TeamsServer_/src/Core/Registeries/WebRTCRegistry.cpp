#include "WebRTCRegistry.h"
#include "../../Network/Session/WebRTCSession.h"


void WebRTCRegistry::register_session(const std::string& user_uuid,
                                       std::shared_ptr<WebRTCSession> session) {
    std::lock_guard lock(mutex_);
    webRTCSessions_[user_uuid] = std::move(session);
}

void WebRTCRegistry::unregister_session(const std::string& user_uuid) {
    std::lock_guard lock(mutex_);
    webRTCSessions_.erase(user_uuid);
}

bool WebRTCRegistry::sendMessage(const std::string& user_uuid, const std::string& payload) {
    std::lock_guard lock(mutex_);
    auto it = webRTCSessions_.find(user_uuid);

    if (it == webRTCSessions_.end()) {
        std::cout << "[WebRTCRegistry] No session found for user UUID (probably offline): " << user_uuid << std::endl;
        return false;
    }
    it->second->send(payload);
    std::cout << "[WebRTCRegistry] Message sent to user UUID: " << user_uuid << std::endl;
    return true;
}

bool WebRTCRegistry::isConnected(const std::string& user_uuid) const {
    std::lock_guard lock(mutex_);
    return webRTCSessions_.contains(user_uuid);
}