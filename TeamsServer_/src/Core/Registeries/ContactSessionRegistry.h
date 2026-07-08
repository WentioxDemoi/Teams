#ifndef CONTACT_SESSION_REGISTRY_H
#define CONTACT_SESSION_REGISTRY_H

#include "../Models/Message.h"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

class ContactSession;

class ContactSessionRegistry {
public:
    void registerContactSession(const std::string& user_uuid,
                                std::shared_ptr<ContactSession> session);

    void unregisterContactSession(const std::string& user_uuid);

    bool sendMessage(const std::string& user_uuid, const std::string& payload);

    bool hasContactSession(const std::string& user_uuid) const;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<ContactSession>> contactSessions_;
};

#endif