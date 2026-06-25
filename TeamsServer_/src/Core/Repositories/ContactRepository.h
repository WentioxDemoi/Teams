#ifndef CONTACTREPOSITORY_H
#define CONTACTREPOSITORY_H

#include "../../Infrastructure/Database/DatabaseManager.h"
#include "../../Infrastructure/Database/QueryBuilder.h"
#include "../../Utils/Config.h"

#include "../Models/Contact.h"
#include "../Models/User.h"

class ContactRepository {
public:
  ContactRepository() : databaseManager_(DatabaseManager::instance()), config_(Config::instance()) {}

  virtual bool create(const Contact &contact);

  virtual bool remove(const std::string &userUuid, const std::string &contactUuid);

  virtual std::vector<User> find_contacts(const std::string &userUuid);
  virtual bool update_last_read_at(const std::string &userUuid, const std::string &contactUuid,
                                   const std::string &lastReadAt);
  virtual ~ContactRepository() = default;

private:
  Config &config_;
  DatabaseManager &databaseManager_;
};

#endif