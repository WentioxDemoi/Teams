#ifndef INETWORKSERVICE_H
#define INETWORKSERVICE_H

#include "SessionEnum.h"

#include <QJsonObject>
#include <QObject>
#include <QString>

class INetworkService : public QObject {
  Q_OBJECT

 public:
  explicit INetworkService(QObject* parent = nullptr) : QObject(parent) {}
  ~INetworkService() override = default;

  virtual void send(const QJsonObject& payload) = 0;
  virtual void disconnectFromServer() = 0;

 signals:
  void jsonReceived(const QJsonObject& json);
  void networkError(const QString& error);
  void connectionUpdate(ServerType server, bool status);
};

#endif