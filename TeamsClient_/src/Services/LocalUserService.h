#ifndef USERSERVICE_H
#define USERSERVICE_H

#include "../Models/User.h"
#include "../Repositories/UserRepository.h"
#include "Core/State/UserState.h"
#include "Interfaces/ILocalUserService.h"

/**
 * @class UserService
 * @brief Service de gestion des utilisateurs.
 *
 * Fournit des méthodes pour sauvegarder, supprimer un utilisateur ou tous les utilisateurs,
 * en s'appuyant sur le UserRepository pour l'accès aux données.
 */
// LocalUserService.h (renommé depuis UserService)
class LocalUserService : public ILocalUserService {
  Q_OBJECT
public:
  explicit LocalUserService(UserState *userState = nullptr, UserRepository *userRepo = nullptr, QObject *parent = nullptr);

  void saveLocalUser(const User &user) override;
  void deleteAll() override;

signals:
  void localUserSaved(const User &user);

private:
  UserState *userState_;
  UserRepository userRepo_;
};

#endif