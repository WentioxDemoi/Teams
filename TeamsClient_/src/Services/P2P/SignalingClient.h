#ifndef SIGNALINGCLIENT_H
#define SIGNALINGCLIENT_H

#include "../Network/NetworkService.h"
#include <QObject>

class SignalingClient : public QObject {
  Q_OBJECT
 public:
  explicit SignalingClient(QObject* parent = nullptr);

  void sendOffer(const QString& sdp);
  void sendAnswer(const QString& sdp);
  void sendIce(const QString& payload);
  void disconnectFromServer();

 signals:
  void offerReceived(QString sdp);
  void answerReceived(QString sdp);
  void iceReceived(QString candidate, QString mid, int index);

 private slots:
  void handleServerResponse(const QJsonObject& root);

 public slots:
  void registerWithServer4WebRTC(QString UUID); // A refactor

 private:
  NetworkService* network_;
  QString clientId_; // A refactor

  void sendMessage(const QString& type, const QString& payload);
};

#endif