#ifndef AUTHNETWORKSERVICE_H
#define AUTHNETWORKSERVICE_H

#include "Interfaces/IAuthNetworkService.h"

/**
 * @class AuthNetworkService
 * @brief Service réseau pour gérer les requêtes d'authentification.
 *
 * Envoie les requêtes login et register au serveur via QSslSocket,
 * reçoit et parse les réponses JSON, et émet les signaux authSuccess ou authError
 * selon le résultat.
 */
class AuthNetworkService : public IAuthNetworkService
{
  Q_OBJECT
public:
  explicit AuthNetworkService(QObject *parent = nullptr);

  void loginUser(const QString &email, const QString &password) override;
  void registerUser(const QString &email, const QString &username,
                    const QString &password) override;
  
  void handleServerResponse(const QByteArray &data) override;
  void validateToken(const QString &value) override;
    

private:
  QByteArray buffer_;
  void sendRequest(const QJsonObject &payload) override;
  void sendPendingPayload() override;

  QSslSocket socket_;
  QJsonObject pendingPayload_;
  bool waitingForResponse_;
};

#endif