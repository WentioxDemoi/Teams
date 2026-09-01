#ifndef CONTACTSERVICE_H
#define CONTACTSERVICE_H

#include "IContactService.h"
#include "../Repositories/ContactRepository.h"
// #include "../../Core/Registeries/ContactSessionRegistry.h"
#include "../Models/Contact.h"
#include "Registeries/ContactSessionRegistry.h"
#include "Repositories/UserRepository.h"
#include <memory>
#include <vector>

class ContactService : public IContactService {
 public:
  ContactService(std::unique_ptr<ContactRepository> contactRepo,
                 std::shared_ptr<IUserRepository> userRepo,
                 std::shared_ptr<ContactSessionRegistry> contactSessionRegistry)
      : contactRepo_(std::move(contactRepo)),
        userRepo_(userRepo),
        contactSessionRegistry_(contactSessionRegistry),
        config_(Config::instance()) {}

  ~ContactService() override = default;

  std::optional<std::string> addContact(const std::string &payload) override;
  std::optional<std::string> removeContact(const std::string &payload) override;
  std::optional<std::string> loadContacts(const std::string &payload) override;
  std::optional<std::string> searchUsers(const std::string &userUuid, const std::string &payload) override;
  std::optional<std::string> resolveUserByUuid(const std::string &uuid) override;
  std::optional<std::string> lastReadAt(const std::string &uuid, const std::string &timestamp) override;
  std::optional<std::string> updateStatus(const std::string &userUuid, const std::string &payload) override;

 private:
  std::unique_ptr<ContactRepository> contactRepo_;
  std::shared_ptr<ContactSessionRegistry> contactSessionRegistry_;
  std::shared_ptr<IUserRepository> userRepo_;
  Config &config_;
};

#endif