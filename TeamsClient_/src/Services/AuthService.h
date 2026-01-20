#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../Core/ServiceLocator.h"
#include "../Models/User.h"
#include "../includes.h"
#include "AuthNetworkService.h"
#include "Database/DBService.h"
#include "IAuthService.h"
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
  explicit AuthService(QObject *parent = nullptr);


public slots:
  
  void onUserSaved(const User &user);
  // Auth classique
  void initClassicAuth();
  void loginUser(const QString &username, const QString &password) override;

  void registerUser(const QString &email, const QString &username,
                    const QString &password) override;

  // Auth via Token
  void initTokenAuth();
  void errorToken(const QString &errorText);
  

private:
  AuthNetworkService *network_;
  TokenManager *token_;
  DBService *db_;
};

#endif