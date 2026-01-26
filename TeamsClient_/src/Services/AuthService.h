#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../Core/ServiceLocator.h"
#include "../Models/User.h"
#include "../includes.h"
#include "AuthNetworkService.h"
#include "UserService.h"
#include "Interfaces/IAuthService.h"
#include "../Utils/TokenManager.h"

/**
 * @class AuthService
 * @brief Gère l'authentification des utilisateurs via AuthNetworkService.
 *
 * Implémente IAuthService. Émet authSuccess ou authError selon le résultat.
 */
class AuthService : public IAuthService
{
  Q_OBJECT

public:
  explicit AuthService(AuthNetworkService* network = nullptr,
                         UserService* userService = nullptr,
                         TokenManager* token = nullptr,
                         QObject* parent = nullptr);
  void start() override;
  void loginUser(const QString &username, const QString &password) override;

  void registerUser(const QString &email, const QString &username,
                    const QString &password) override;

public slots:
  void onUserSaved(const User &user);
  void errorToken(const QString &error);
  void errorUserService(const QString &error);
  

private:

  AuthNetworkService *network_;
  TokenManager *token_;
  UserService *userService_;
};

#endif