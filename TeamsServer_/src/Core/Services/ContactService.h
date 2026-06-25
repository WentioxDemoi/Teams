#ifndef CONTACTSERVICE_H
#define CONTACTSERVICE_H

#include "../Repositories/ContactRepository.h"
// #include "../../Core/Registeries/ContactSessionRegistry.h"
#include "../Models/Contact.h"
#include "Registeries/ContactSessionRegistry.h"
#include "Repositories/UserRepository.h"
#include <memory>
#include <vector>

class ContactService {
public:
  ContactService(std::unique_ptr<ContactRepository> contactRepo,
                 std::shared_ptr<UserRepository> userRepo,
                std::shared_ptr<ContactSessionRegistry> contactSessionRegistry)
      : contactRepo_(std::move(contactRepo)), userRepo_(userRepo), contactSessionRegistry_(contactSessionRegistry),
        config_(Config::instance()) {}

  virtual ~ContactService() = default;

  virtual std::optional<std::string> addContact(const std::string &payload);
  virtual std::optional<std::string> removeContact(const std::string &payload);
  virtual std::optional<std::string> loadContacts(const std::string &payload);
  virtual std::optional<std::string> searchUsers(const std::string &userUuid, const std::string &payload);
  virtual std::optional<std::string> resolveUserByUuid(const std::string &uuid);
  virtual std::optional<std::string> lastReadAt(const std::string &uuid, const std::string &timestamp);
  virtual std::optional<std::string> updateStatus(const std::string &userUuid, const std::string &payload);

private:
  std::unique_ptr<ContactRepository> contactRepo_;
    std::shared_ptr<ContactSessionRegistry> contactSessionRegistry_;
  std::shared_ptr<UserRepository> userRepo_;
  Config &config_;
};

#endif