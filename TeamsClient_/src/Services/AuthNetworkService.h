#ifndef AUTHNETWORKSERVICE_H
#define AUTHNETWORKSERVICE_H

#include "../Models/User.h"
#include "../includes.h"

/**
 * @class AuthNetworkService
 * @brief Service réseau pour gérer les requêtes d'authentification.
 *
 * Envoie les requêtes login et register au serveur via QSslSocket,
 * reçoit et parse les réponses JSON, et émet les signaux authSuccess ou authError
 * selon le résultat.
 */
class AuthNetworkService : public QObject
{
  Q_OBJECT
public:
  explicit AuthNetworkService(QObject *parent = nullptr);

  void loginUser(const QString &email, const QString &password);
  void registerUser(const QString &email, const QString &username,
                    const QString &password);
  
  void handleServerResponse(const QByteArray &data);

signals:
  void authSuccess(const User &user);
  void authError(const QString &message);
  void invalidToken(const QString &error);

public slots:
  void validateToken(const QString &key, const QString &value);
    

private:
  QByteArray buffer_;
  void sendRequest(const QJsonObject &payload);
  void sendPendingPayload();

  QSslSocket socket_;
  QJsonObject pendingPayload_;
  bool waitingForResponse_;
};

#endif