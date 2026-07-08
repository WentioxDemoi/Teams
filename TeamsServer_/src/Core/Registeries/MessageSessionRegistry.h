#ifndef SESSION_REGISTRY_H
#define SESSION_REGISTRY_H
#include "../Models/Message.h"
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

class MessageSession;

class MessageSessionRegistry {
public:
    void registerMessageSession(const std::string& user_uuid,
                                std::shared_ptr<MessageSession> session);
    void unregisterMessageSession(const std::string& user_uuid);
    bool sendMessage(const std::string& user_uuid, const std::string& payload);
    bool hasMessageSession(const std::string& user_uuid) const;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<MessageSession>> messageSessions_;
};
#endif