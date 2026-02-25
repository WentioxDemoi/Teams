#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../Core/ServiceLocator.h"
#include "../Models/User.h"
#include "../includes.h"
#include "Interfaces/IAuthNetworkService.h"
#include "Interfaces/IUserService.h"
#include "Interfaces/IAuthService.h"
#include "AuthNetworkService.h"
#include "UserService.h"
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
  explicit AuthService(IAuthNetworkService* network = nullptr,
                         IUserService* userService = nullptr,
                         ITokenManager* token = nullptr,
                         QObject* parent = nullptr);
  void start() override;
  void loginUser(const QString &username, const QString &password) override;

  void registerUser(const QString &email, const QString &username,
                    const QString &password) override;

public slots:
  void onUserSaved(const User &user) override;
  void errorToken(const QString &error) override;
  void errorUserService(const QString &error) override;
  

private:

  IAuthNetworkService *network_;
  ITokenManager *token_;
  IUserService *userService_;
};

#endif