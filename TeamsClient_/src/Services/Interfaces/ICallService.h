#ifndef ICALLSERVICE_H
#define ICALLSERVICE_H

#include <QObject>
#include <QString>
#include "SessionEnum.h"

/**
 * @class ICallService
 * @brief Interface pour la gestion des appels.
 *
 * Définit l'API que doit exposer un service d'appel pour démarrer,
 * accepter, raccrocher ou fermer la connexion au serveur d'appel.
 */
class ICallService : public QObject {
  Q_OBJECT
 public:
  explicit ICallService(QObject* parent = nullptr) : QObject(parent) {};
  virtual ~ICallService() = default;

 public slots:
  virtual void startCall(const QString &contactUuid, const QString &contactUsername) = 0;
  virtual void acceptCall(const QString &remoteUsername) = 0;
  virtual void cameraEnabledChanged(bool cameraEnabled) = 0;
  virtual void hangup() = 0;
  virtual void disconnectFromServer() = 0;
  virtual void rejectCall() = 0;


 signals:
  void callStarted(const QString& callUuid, const QString& calleeUuid);
  void callAccepted(const QString& callUuid, const QString& callerUuid);
  void callEnded(const QString& callUuid, const QString& reason);
  void callError(const QString& error);
  void connectionUpdate(ServerType server, bool status);

  void offerReceived(QString sdp);
  void answerReceived(QString sdp);
  void iceReceived(QString candidate, QString mid, int index);

  void isContactConnectedChanged(bool isConnected);
  void onCameraEnabledChanged(bool cameraEnabled);

    // Émis côté callee quand une offer arrive et qu'on doit afficher le popup d'appel entrant.
 void incomingCallReceived(const QString &callerUuid);

  // Émis côté callee si le caller annule avant que l'utilisateur ait répondu au popup.
  void incomingCallCancelled(const QString &callerUuid);
  // Émis côté caller une fois que le serveur a confirmé que le callee est joignable;
  // déclenche WebRTCService::createOffer() en aval.
  void triggerCreateOffer();
    void openCallWindow(const QString &remoteUsername);
    void closeCallWindow();

};

#endif
