#ifndef CALLSERVICE_H
#define CALLSERVICE_H

#include <QJsonObject>
#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QtCore/qobjectdefs.h>
#include "../Interfaces/ICallService.h"
#include "../Network/NetworkService.h"
#include "P2P/WebRTCService.h"
#include "WebRTCEnum.h"
#include <QTimer>


/**
 * @class CallService
 * @brief Service de gestion des appels.
 *
 * Envoie et reçoit les événements d'appel via NetworkService sur le port 8083.
 */
class CallService : public ICallService {
  Q_OBJECT

 public:
  explicit CallService(NetworkService *network = nullptr,
                        WebRTCService *webRTCService = nullptr, QObject *parent = nullptr);



 public slots:
  void startCall(const QString &contactUuid, const QString &contactUsername) override;
  void hangup() override;
  void disconnectFromServer() override;
  void acceptCall(const QString &remoteUsername) override;
  void cameraEnabledChanged(bool cameraEnabled) override;
  void rejectCall() override;

 private slots:
  void startCallTimeoutTimer();
  void sendSignalingMessage(WebRTCMessageType type, const QJsonObject &data);
  void handleServerResponse(const QJsonObject& root);

 signals:


 private:
  QString remoteUuid_;
  QString remoteUsername_;
  QString pendingOfferSdp_;

  NetworkService* network_;
  WebRTCService *webRTCService_;

  bool inCall_ = false;


  QTimer *callTimeoutTimer_ = nullptr;

};

#endif