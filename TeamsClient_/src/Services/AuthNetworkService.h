#ifndef AUTHNETWORKSERVICE_H
#define AUTHNETWORKSERVICE_H

#include "../Models/User.h"
#include "../includes.h"

class AuthNetworkService : public QObject {
  Q_OBJECT
public:
  explicit AuthNetworkService(QObject *parent = nullptr);

  void loginUser(const QString &email, const QString &password);
  void registerUser(const QString &email, const QString &username,
                    const QString &password);

signals:
  void authSuccess(const User &user);
  void authError(const QString &message);

private:
  void sendRequest(const QJsonObject &payload);
  void sendPendingPayload();

  QSslSocket socket_;
  QJsonObject pendingPayload_;
  bool waitingForResponse_;
};

#endif