#ifndef IMESSAGE_SESSION_REGISTRY_H
#define IMESSAGE_SESSION_REGISTRY_H

#include <memory>
#include <string>

class MessageSession;

class IMessageSessionRegistry {
public:
  virtual ~IMessageSessionRegistry() = default;

  virtual void registerMessageSession(const std::string &user_uuid,
                                       std::shared_ptr<MessageSession> session) = 0;

  virtual void unregisterMessageSession(const std::string &user_uuid) = 0;

  virtual bool sendMessage(const std::string &user_uuid, const std::string &payload) = 0;

  virtual bool hasMessageSession(const std::string &user_uuid) const = 0;
};

#endif