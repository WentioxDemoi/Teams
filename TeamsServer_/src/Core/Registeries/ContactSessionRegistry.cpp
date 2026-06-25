#include "ContactSessionRegistry.h"
#include "../../Network/Session/MessageSession.h"
#include "Session/ContactSession.h"

void ContactSessionRegistry::registerContactSession(const std::string &user_uuid,
                                                    std::shared_ptr<ContactSession> session) {

  std::lock_guard lock(mutex_);
  contactSessions_[user_uuid] = std::move(session);
}

void ContactSessionRegistry::unregisterContactSession(const std::string &user_uuid) {

  std::lock_guard lock(mutex_);
  contactSessions_.erase(user_uuid);
}

bool ContactSessionRegistry::sendMessage(const std::string &user_uuid, const std::string &payload) {

  std::lock_guard lock(mutex_);

  auto it = contactSessions_.find(user_uuid);

  if (it == contactSessions_.end()) {
    std::cout << "[ContactSessionRegistry] No session found for user UUID: " << user_uuid << std::endl;
    return false;
  }

  it->second->send(payload);

  std::cout << "[ContactSessionRegistry] Message sent to user UUID: " << user_uuid << std::endl;

  return true;
}

bool ContactSessionRegistry::hasContactSession(const std::string &user_uuid) const {

  std::lock_guard lock(mutex_);
  return contactSessions_.contains(user_uuid);
}