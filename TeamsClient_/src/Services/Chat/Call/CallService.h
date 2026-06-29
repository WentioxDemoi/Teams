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
  void acceptCall() override;
  void hangup() override;
  void disconnectFromServer() override;
  void rejectCall() override;

 private slots:
  void startCallTimeoutTimer();
  void sendSignalingMessage(WebRTCMessageType type, const QJsonObject &data);
  void handleServerResponse(const QJsonObject& root);

 signals:
  // Émis côté callee quand une offer arrive et qu'on doit afficher le popup d'appel entrant.
  void incomingCallReceived(const QString &callerUuid);
  // Émis côté callee si le caller annule avant que l'utilisateur ait répondu au popup.
  void incomingCallCancelled(const QString &callerUuid);
  // Émis côté caller une fois que le serveur a confirmé que le callee est joignable;
  // déclenche WebRTCService::createOffer() en aval.
  void triggerCreateOffer();

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