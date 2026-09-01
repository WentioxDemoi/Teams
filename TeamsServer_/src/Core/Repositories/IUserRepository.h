#ifndef IUSERREPOSITORY_H
#define IUSERREPOSITORY_H

#include "../Models/User.h"
#include <optional>
#include <string>
#include <vector>

/**
 * @class IUserRepository
 * @brief Interface d'accès aux données des utilisateurs.
 *
 * Définit le contrat que toute implémentation d'un repository utilisateur
 * doit respecter, indépendamment de la technologie de persistence utilisée.
 */
class IUserRepository {
public:
  virtual ~IUserRepository() = default;

  virtual std::optional<User> find_by_uuid(const std::string &uuid) = 0;
  virtual std::optional<User> find_by_email(const std::string &email) = 0;
  virtual std::optional<User> find_by_token(const std::string &token) = 0;

  virtual std::vector<User> search_by_name(
      const std::string &callerUuid,
      const std::string &name) = 0;

  virtual bool create(const User &user) = 0;
  virtual bool update(const User &user) = 0;
  virtual bool delete_user(const std::string &uuid) = 0;
};

#endif