#ifndef CONTACT_SESSION_REGISTRY_H
#define CONTACT_SESSION_REGISTRY_H

#include <mutex>
#include <unordered_map>

#include "../Models/Message.h"
#include "IContactSessionRegistry.h"

class ContactSession;

class ContactSessionRegistry : public IContactSessionRegistry {
 public:
  void registerContactSession(const std::string& user_uuid,
                              std::shared_ptr<ContactSession> session) override;

  void unregisterContactSession(const std::string& user_uuid) override;

  bool sendMessage(const std::string& user_uuid, const std::string& payload) override;

  bool hasContactSession(const std::string& user_uuid) const override;

 private:
  mutable std::mutex mutex_;
  std::unordered_map<std::string, std::shared_ptr<ContactSession>> contactSessions_;
};

#endif