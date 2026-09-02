#ifndef CONTACTREPOSITORY_H
#define CONTACTREPOSITORY_H

#include "../../Infrastructure/DatabaseManager.h"
#include "../../Infrastructure/QueryBuilder.h"
#include "../../Utils/Config.h"
#include "../Models/Contact.h"
#include "../Models/User.h"
#include "IContactRepository.h"

class ContactRepository : public IContactRepository {
 public:
  ContactRepository()
      : databaseManager_(DatabaseManager::instance()), config_(Config::instance()) {}

  bool create(const Contact& contact) override;

  bool remove(const std::string& userUuid, const std::string& contactUuid) override;

  std::vector<User> find_contact_owners(const std::string& userUuid) override;
  std::vector<User> find_contacts(const std::string& userUuid) override;

  bool update_last_read_at(const std::string& userUuid, const std::string& contactUuid,
                           const std::string& lastReadAt) override;

  ~ContactRepository() override = default;

 private:
  Config& config_;
  DatabaseManager& databaseManager_;
};

#endif