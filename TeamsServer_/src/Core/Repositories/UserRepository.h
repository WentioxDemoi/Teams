#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include "../../Infrastructure/Database/DatabaseManager.h"
#include "../../Infrastructure/Database/QueryBuilder.h"
#include "../../Utils/Config.h"
#include "../../includes.h"
#include "../Models/User.h"

/**
 * @class UserRepository
 * @brief Gestionnaire d'accès aux données des utilisateurs dans la base de
 * données.
 *
 * Fournit des méthodes pour rechercher, créer, mettre à jour et supprimer des
 * utilisateurs. Utilise DatabaseManager pour interagir avec la base de données
 * et Config pour les paramètres de configuration liés à la persistence.
 *
 * Assure l'encapsulation de l'accès à la base de données pour le module
 * utilisateur.
 */
class UserRepository {
public:
  UserRepository()
      : databaseManager_(DatabaseManager::instance()),
        config_(Config::instance()) {};

  virtual std::optional<User> find_by_uuid(const std::string &uuid);
  virtual std::optional<User> find_by_email(const std::string &email);
  virtual std::optional<User> find_by_token(const std::string &token);

  virtual bool create(const User &user);
  virtual bool update(const User &user);
  virtual bool delete_user(const std::string &uuid);

  virtual ~UserRepository() = default;

private:
  User row_to_user(const pqxx::row &row);

  Config &config_;
  DatabaseManager &databaseManager_;
};

#endif