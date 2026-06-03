#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../../Models/User.h"
#include "../../Utils/Interfaces/ITokenManager.h"
#include "../Interfaces/IAuthService.h"
#include "../Interfaces/IUserService.h"
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
  explicit AuthService(NetworkService* network = nullptr, IUserService* userService = nullptr,
                       ITokenManager* token = nullptr, QObject* parent = nullptr);
  void start() override;
  void loginUser(const QString& username, const QString& password) override;

  void registerUser(const QString& firstName, const QString& lastName, const QString& email,
                    const QString& password) override;

  void disconnectFromServer() override;

 public slots:
  void onUserSaved(const User& user) override;
  void errorToken(const QString& error) override;
  void handleServerResponse(const QJsonObject& root);

 private:
  NetworkService* network_;
  ITokenManager* token_;
  IUserService* userService_;
};

#endif