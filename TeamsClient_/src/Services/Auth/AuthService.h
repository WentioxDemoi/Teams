#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../../Models/User.h"
#include "../Interfaces/IAuthService.h"
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
  explicit AuthService(NetworkService* network = nullptr, QObject* parent = nullptr);
  void loginUser(const QString& username, const QString& password) override;

  void registerUser(const QString& firstName, const QString& lastName, const QString& email,
                    const QString& password) override;
  void loginWithToken(const QString& token) override;

  void disconnectFromServer() override;

 public slots:
  void handleServerResponse(const QJsonObject& root);

 private:
  NetworkService* network_;
};

#endif