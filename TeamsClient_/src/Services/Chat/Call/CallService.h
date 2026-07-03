#ifndef CALLSERVICE_H
#define CALLSERVICE_H

#include "Interfaces/ICallService.h"
#include "Network/NetworkService.h"
#include "P2P/WebRTCService.h"
#include "WebRTCEnum.h"

#include <QJsonObject>
#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QTimer>
#include <QtCore/qobjectdefs.h>

/**
 * @class CallService
 * @brief Service de gestion des appels.
 *
 * Envoie et reçoit les événements d'appel via NetworkService (port 8083)
 * et pilote WebRTCService pour la négociation SDP/ICE.
 */
class CallService : public ICallService {
  Q_OBJECT

 public:
  explicit CallService(NetworkService *network = nullptr,
                        WebRTCService *webRTCService = nullptr,
                        QObject *parent = nullptr);

  // API d'appel (ICallService)
  void startCall(const QString &contactUuid, const QString &contactUsername) override;
  void hangup() override;
  void acceptCall(const QString &remoteUsername) override;
  void rejectCall() override;
  void cameraEnabledChanged(bool cameraEnabled) override;
  void disconnectFromServer() override;

  // Interne : signaling et timeout
 private:
  void handleServerResponse(const QJsonObject &root);
  void sendSignalingMessage(WebRTCMessageType type, const QJsonObject &data);
  void startCallTimeoutTimer();

  // Dépendances
  NetworkService *network_;
  WebRTCService *webRTCService_;

  // État de l'appel en cours
  bool inCall_ = false;
  QString remoteUuid_;
  QString remoteUsername_;
  QString pendingOfferSdp_;

  // Timeout "pas de réponse" côté caller
  QTimer *callTimeoutTimer_ = nullptr;
};

#endif