#ifndef IAUTHSERVICE_H
#define IAUTHSERVICE_H

#include <optional>
#include <string>

#include "../Models/User.h"

/**
 * @class IAuthService
 * @brief Contrat métier pour l'authentification.
 *
 * Centralise les opérations d'authentification et permet d'injecter une
 * implémentation concrète sans coupler le reste de l'application.
 */
class IAuthService {
 public:
  virtual ~IAuthService() = default;

  virtual std::optional<User> loginUser(const User& user) = 0;
  virtual std::optional<User> registerUser(const User& user) = 0;
  virtual std::optional<User> validateToken(const std::string& token) = 0;
};

#endif
