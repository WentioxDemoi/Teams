#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../../Utils/Crypto.h"
#include "../Repositories/UserRepository.h"

/**
 * @class AuthService
 * @brief Service de gestion de l'authentification des utilisateurs.
 *
 * Fournit des fonctionnalités pour connecter, enregistrer et valider les
 * utilisateurs. S'appuie sur UserRepository pour accéder aux données
 * utilisateur et Config pour les paramètres de configuration liés à
 * l'authentification. Gère la sécurité des mots de passe via Crypto et
 * encapsule la logique métier liée à l'authentification.
 */
class AuthService {
public:
  AuthService(std::unique_ptr<UserRepository> userRepo)
      : userRepo_(std::move(userRepo)), config_(Config::instance()) {};
  virtual ~AuthService() = default;

  virtual std::optional<User> loginUser(const User &user);
  virtual std::optional<User> registerUser(const User &user);
  virtual std::optional<User> validateToken(const std::string &token);

private:
  std::unique_ptr<UserRepository> userRepo_;
  Config &config_;
};

#endif