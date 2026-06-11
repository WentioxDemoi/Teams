#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../../Models/User.h"
#include "../Interfaces/IAuthService.h"
#include "../Interfaces/IUserService.h"
#include "../../Utils/Interfaces/ITokenManager.h"
#include "Network/NetworkService.h"
#include "Interfaces/IMessageService.h"


/**
 * @class AuthService
 * @brief Gère l'authentification des utilisateurs via NetworkService.
 *
 * Implémente IAuthService. Émet authSuccess ou authError selon le résultat.
 */
class AuthService : public IAuthService {
  Q_OBJECT

 public:
  explicit AuthService(NetworkService* network = nullptr,
                       IUserService* userService = nullptr,
                       IMessageService* messageService = nullptr,
                       ITokenManager* tokenManager = nullptr,
                       QObject* parent = nullptr);
  void loginUser(const QString& username, const QString& password) override;

  void registerUser(const QString& firstName, const QString& lastName, const QString& email,
                    const QString& password) override;
  void loginWithToken(void) override;

  void disconnectFromServer() override;

 public slots:
  void handleServerResponse(const QJsonObject& root);

 private slots:
  void handleLocalUserSaved(const User& user);
  void handleTokenError(const QString& error);

 private:
  NetworkService* network_;
  IUserService* userService_;
  IMessageService* messageService_;
  ITokenManager* tokenManager_;
};

#endif