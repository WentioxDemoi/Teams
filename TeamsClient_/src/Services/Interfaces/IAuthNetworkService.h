#ifndef IAUTHNETWORKSERVICE_H
#define IAUTHNETWORKSERVICE_H

#include "../../Models/User.h"
#include "../../includes.h"
#include <QtCore/qjsonobject.h>

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
  
  virtual void handleServerResponse(const QJsonObject &data) = 0;
  virtual void validateToken(const QString &value) = 0;
  virtual void disconnectFromServer() = 0;


signals:
  void authSuccess(const User &user);
  void authError(const QString &message);
  void invalidToken(const QString &error);
  void registerWithServer4WebRTC(QString UUID);
    

private:
  QByteArray buffer_;
  // virtual void sendRequest(const QJsonObject &payload);
  // virtual void sendPendingPayload();

  QSslSocket socket_;
  QJsonObject pendingPayload_;
  bool waitingForResponse_;
};

#endif