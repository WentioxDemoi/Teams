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
  virtual void startCall(const QString& calleeUuid) = 0;
  virtual void acceptCall(const QString& callUuid) = 0;
  virtual void hangup(const QString& callUuid) = 0;
  virtual void disconnectFromServer() = 0;

 signals:
  void callStarted(const QString& callUuid, const QString& calleeUuid);
  void callAccepted(const QString& callUuid, const QString& callerUuid);
  void callEnded(const QString& callUuid, const QString& reason);
  void callError(const QString& error);
  void connectionUpdate(ServerType server, bool status);
};

#endif
