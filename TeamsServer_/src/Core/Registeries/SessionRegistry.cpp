#include "SessionRegistry.h"

void SessionRegistry::register_session(const std::string& user_uuid,
                                       std::shared_ptr<MessageSession> session) {
    std::lock_guard lock(mutex_);
    sessions_[user_uuid] = std::move(session);
}

void SessionRegistry::unregister_session(const std::string& user_uuid) {
    std::lock_guard lock(mutex_);
    sessions_.erase(user_uuid);
}

bool SessionRegistry::send_to(const std::string& user_uuid, const std::string& payload) {
    std::lock_guard lock(mutex_);
    auto it = sessions_.find(user_uuid);
    if (it == sessions_.end()) return false;
    it->second->send(payload);
    return true;
}

bool SessionRegistry::has_session(const std::string& user_uuid) const {
    std::lock_guard lock(mutex_);
    return sessions_.contains(user_uuid);
}