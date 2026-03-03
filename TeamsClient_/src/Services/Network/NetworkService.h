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

/**
 * @class NetworkService
 * @brief Service de communication réseau sécurisé.
 *
 * Gère l'envoi et la réception de données JSON via SSL, assure la connexion
 * au serveur et signale les erreurs réseau ou les messages reçus à l'application.
 */
class NetworkService : public QObject {
  Q_OBJECT

 public:
  explicit NetworkService(qint16 port, QObject* parent = nullptr);

  void send(const QJsonObject& payload);
  void disconnectFromServer();

 signals:
  void jsonReceived(const QJsonObject& json);
  void networkError(const QString& error);

 private:
  void handleIncomingData(const QByteArray& data);
  void ensureConnected();

 private:
  QSslSocket socket_;
  QByteArray buffer_;
  qint16 port_;
};

#endif