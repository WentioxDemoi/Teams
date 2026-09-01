#ifndef ICONTACT_SESSION_REGISTRY_H
#define ICONTACT_SESSION_REGISTRY_H

#include <memory>
#include <string>

class ContactSession;

class IContactSessionRegistry {
public:
  virtual ~IContactSessionRegistry() = default;

  virtual void
  registerContactSession(const std::string &user_uuid,
                         std::shared_ptr<ContactSession> session) = 0;

  virtual void unregisterContactSession(const std::string &user_uuid) = 0;

  virtual bool sendMessage(const std::string &user_uuid,
                           const std::string &payload) = 0;

  virtual bool hasContactSession(const std::string &user_uuid) const = 0;
};

#endif