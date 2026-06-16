#include "MessageSessionRegistry.h"
#include "../../Network/Session/MessageSession.h"


void MessageSessionRegistry::registerMessageSession(const std::string& user_uuid,
                                       std::shared_ptr<MessageSession> session) {
    std::lock_guard lock(mutex_);
    messageSessions_[user_uuid] = std::move(session);
}

void MessageSessionRegistry::unregisterMessageSession(const std::string& user_uuid) {
    std::lock_guard lock(mutex_);
    messageSessions_.erase(user_uuid);
}

bool MessageSessionRegistry::sendMessage(const std::string& user_uuid, const std::string& payload) {
    std::lock_guard lock(mutex_);
    auto it = messageSessions_.find(user_uuid);
    if (it == messageSessions_.end()) return false;
    it->second->send(payload);
    return true;
}

bool MessageSessionRegistry::hasMessageSession(const std::string& user_uuid) const {
    std::lock_guard lock(mutex_);
    return messageSessions_.contains(user_uuid);
}