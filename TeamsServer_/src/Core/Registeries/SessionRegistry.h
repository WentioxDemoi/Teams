#ifndef SESSION_REGISTRY_H
#define SESSION_REGISTRY_H
#include "../../Network/Session/MessageSession.h"
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

class SessionRegistry {
public:
    void register_session(const std::string& user_uuid,
                          std::shared_ptr<MessageSession> session);
    void unregister_session(const std::string& user_uuid);
    bool send_to(const std::string& user_uuid, const std::string& payload);
    bool has_session(const std::string& user_uuid) const;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<MessageSession>> sessions_;
};
#endif