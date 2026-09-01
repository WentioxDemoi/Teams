#ifndef SESSION_REGISTRY_H
#define SESSION_REGISTRY_H
#include "IMessageSessionRegistry.h"
#include "../Models/Message.h"
#include <mutex>
#include <unordered_map>

class MessageSession;

class MessageSessionRegistry : public IMessageSessionRegistry {
public:
    void registerMessageSession(const std::string& user_uuid,
                                std::shared_ptr<MessageSession> session) override;
    void unregisterMessageSession(const std::string& user_uuid) override;
    bool sendMessage(const std::string& user_uuid, const std::string& payload) override;
    bool hasMessageSession(const std::string& user_uuid) const override;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<MessageSession>> messageSessions_;
};
#endif