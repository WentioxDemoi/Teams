#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../../Models/User.h"
#include "../../Utils/Interfaces/ITokenManager.h"
#include "../Interfaces/IAuthService.h"
#include "../Interfaces/ILocalUserService.h"
#include "Interfaces/IMessageService.h"
#include "Network/NetworkService.h"

/**
 * @class AuthService
 * @brief Gère l'authentification des utilisateurs via NetworkService.
 *
 * Implémente IAuthService. Émet authSuccess ou authError selon le résultat.
 */
class AuthService : public IAuthService {
  Q_OBJECT

public:
  explicit AuthService(NetworkService *network = nullptr,
                       ILocalUserService *userService = nullptr,
                       IMessageService *messageService = nullptr,
                       ITokenManager *tokenManager = nullptr,
                       QObject *parent = nullptr);
  void loginUser(const QString &username, const QString &password) override;

  void registerUser(const QString &firstName, const QString &lastName,
                    const QString &email, const QString &password) override;
  void loginWithToken(void) override;
  void disconnectFromServer() override;

private:
  void handleServerResponse(const QJsonObject &root);
  void handleLocalUserSaved(const User &user);
  void handleTokenError(const QString &error);

private:
  NetworkService *network_;
  ILocalUserService *localUserService_;
  IMessageService *messageService_;
  ITokenManager *tokenManager_;
};

#endif