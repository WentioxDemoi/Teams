#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../../Utils/Crypto.h"
#include "IAuthService.h"
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
class AuthService : public IAuthService {
 public:
  AuthService(std::shared_ptr<IUserRepository> userRepo)
      : userRepo_(std::move(userRepo)), config_(Config::instance()) {};
  ~AuthService() override = default;

  std::optional<User> loginUser(const User &user) override;
  std::optional<User> registerUser(const User &user) override;
  std::optional<User> validateToken(const std::string &token) override;

 private:
  std::shared_ptr<IUserRepository> userRepo_;
  Config &config_;
};

#endif