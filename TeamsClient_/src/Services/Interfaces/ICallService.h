#ifndef ICALLSERVICE_H
#define ICALLSERVICE_H

#include "SessionEnum.h"
#include <QObject>
#include <QString>

/**
 * @class ICallService
 * @brief Interface pour la gestion des appels (signaling + cycle de vie).
 *
 * Définit l'API que doit exposer un service d'appel pour démarrer,
 * accepter, raccrocher ou fermer la connexion au serveur d'appel.
 */
class ICallService : public QObject {
  Q_OBJECT

public:
  explicit ICallService(QObject *parent = nullptr) : QObject(parent) {}
  virtual ~ICallService() = default;

  // Cycle de vie de l'appel (API appelable)
  virtual void startCall(const QString &contactUuid, const QString &contactUsername) = 0;
  virtual void hangup() = 0;
  virtual void acceptCall(const QString &remoteUsername) = 0;
  virtual void rejectCall() = 0;
  virtual void cameraEnabledChanged(bool cameraEnabled) = 0;
  virtual void disconnectFromServer() = 0;

signals:
  void callError(const QString &error);

  // Notifications d'appel entrant (côté callee)
  void incomingCallReceived(const QString &callerUuid);
  void incomingCallCancelled(const QString &callerUuid);

  // Contrôle de la fenêtre d'appel (UI)
  void openCallWindow(const QString &remoteUsername);
  void closeCallWindow();

  // Négociation WebRTC (signaling SDP / ICE)
  void offerReceived(QString sdp);
  void answerReceived(QString sdp);
  void iceReceived(QString candidate, QString mid, int index);
  void triggerCreateOffer(); // Émis côté caller une fois le callee confirmé joignable.

  // État de l'appel en cours
  void isContactConnectedChanged(bool isConnected);
  void onCameraEnabledChanged(bool cameraEnabled);
};

#endif