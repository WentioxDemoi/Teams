#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include "../../Infrastructure/DatabaseManager.h"
#include "../../Infrastructure/QueryBuilder.h"
#include "../../Utils/Config.h"
#include "../../includes.h"
#include "IUserRepository.h"

/**
 * @class UserRepository
 * @brief Implémentation PostgreSQL du repository utilisateur.
 */
class UserRepository : public IUserRepository {
 public:
  UserRepository() : databaseManager_(DatabaseManager::instance()), config_(Config::instance()) {}

  std::optional<User> find_by_uuid(const std::string& uuid) override;
  std::optional<User> find_by_email(const std::string& email) override;
  std::optional<User> find_by_token(const std::string& token) override;

  std::vector<User> search_by_name(const std::string& callerUuid, const std::string& name) override;

  bool create(const User& user) override;
  bool update(const User& user) override;
  bool delete_user(const std::string& uuid) override;

 private:
  User row_to_user(const pqxx::row& row);

  Config& config_;
  DatabaseManager& databaseManager_;
};

#endif