#ifndef IAUTHNETWORKSERVICE_H
#define IAUTHNETWORKSERVICE_H

#include "../../Models/User.h"
#include "../../includes.h"

/**
 * @class AuthNetworkService
 * @brief Service réseau pour gérer les requêtes d'authentification.
 *
 * Envoie les requêtes login et register au serveur via QSslSocket,
 * reçoit et parse les réponses JSON, et émet les signaux authSuccess ou authError
 * selon le résultat.
 */
class IAuthNetworkService : public QObject
{
  Q_OBJECT
public:
  explicit IAuthNetworkService(QObject *parent = nullptr): QObject(parent) {};

  virtual void loginUser(const QString &email, const QString &password) = 0;
  virtual void registerUser(const QString &email, const QString &username,
                    const QString &password) = 0;
  
  virtual void handleServerResponse(const QByteArray &data) = 0;
  virtual void validateToken(const QString &value) = 0;


signals:
  void authSuccess(const User &user);
  void authError(const QString &message);
  void invalidToken(const QString &error);
    

private:
  QByteArray buffer_;
  virtual void sendRequest(const QJsonObject &payload) = 0;
  virtual void sendPendingPayload() = 0;

  QSslSocket socket_;
  QJsonObject pendingPayload_;
  bool waitingForResponse_;
};

#endif