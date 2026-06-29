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
  virtual void acceptCall() = 0;
  virtual void hangup() = 0;
  virtual void disconnectFromServer() = 0;
  virtual void rejectCall() = 0;

 signals:
 void incomingCallReceived(const QString &callerUuid);
  void callStarted(const QString& callUuid, const QString& calleeUuid);
  void callAccepted(const QString& callUuid, const QString& callerUuid);
  void callEnded(const QString& callUuid, const QString& reason);
  void callError(const QString& error);
  void connectionUpdate(ServerType server, bool status);

  void offerReceived(QString sdp);
  void answerReceived(QString sdp);
  void iceReceived(QString candidate, QString mid, int index);

  void onP2PChange(bool inProgress);

  signals:
    void openCallWindow(const QString &remoteUsername);
    void closeCallWindow();

};

#endif
