#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include <QAbstractSocket>
#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QList>
#include <QObject>
#include <QSslError>
#include <QSslSocket>
#include <QQueue>
#include "INetworkService.h"
#include "User.h"

/**
 * @class NetworkService
 * @brief Service de communication réseau sécurisé.
 *
 * Gère l'envoi et la réception de données JSON via SSL, assure la connexion
 * au serveur et signale les erreurs réseau ou les messages reçus à l'application.
 */
class NetworkService : public INetworkService {
  Q_OBJECT

 public:
  explicit NetworkService(qint16 port, QObject* parent = nullptr);

  Q_INVOKABLE void handleIncomingData(const QByteArray& data);
  int pendingMessageCount() const { return pendingQueue_.size(); }

  void send(const QJsonObject& payload) override;
  void disconnectFromServer() override;

 private:
  void ensureConnected();
  void auth(const User &user);

 private:
  QSslSocket socket_;
  QByteArray buffer_;
  qint16 port_;
  QQueue<QJsonObject> pendingQueue_;
  ServerType server_;
};

#endif