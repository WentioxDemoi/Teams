#ifndef CONTACTSERVICE_H
#define CONTACTSERVICE_H

#include "../Repositories/ContactRepository.h"
// #include "../../Core/Registeries/ContactSessionRegistry.h"
#include "../Models/Contact.h"

class ContactService {
 public:
  ContactService(
      std::unique_ptr<ContactRepository> contactRepo
    //   std::shared_ptr<ContactSessionRegistry> contactSessionRegistry
    )
      : contactRepo_(std::move(contactRepo)),
        // contactSessionRegistry_(contactSessionRegistry),
        config_(Config::instance()) {}

  virtual ~ContactService() = default;

  virtual std::optional<std::string> addContact(const std::string& payload);
  virtual std::optional<std::string> removeContact(const std::string& payload);
  virtual std::optional<std::string> loadContacts(const std::string& payload);

 private:
  std::unique_ptr<ContactRepository> contactRepo_;
//   std::shared_ptr<ContactSessionRegistry> contactSessionRegistry_;
  Config& config_;
};

#endif