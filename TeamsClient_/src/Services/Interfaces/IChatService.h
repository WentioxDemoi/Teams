#ifndef ICHATSERVICE_H
#define ICHATSERVICE_H

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>
#include "Message.h"
#include "SessionEnum.h"
#include "User.h"

/**
 * @class IChatService
 * @brief Interface de haut niveau pour la gestion du chat.
 *
 * Cette interface regroupe les opérations de messagerie et d'appel
 * afin de simplifier l'accès pour les ViewModels.
 */
class IChatService : public QObject {
  Q_OBJECT
 public:
  explicit IChatService(QObject* parent = nullptr) : QObject(parent) {};
  virtual ~IChatService() = default;

 public slots:
  virtual void sendMessage(const QString& recipientUuid, const QString& content) = 0;
  virtual void loadConversationsFromDatabaseAndServer() = 0;
  virtual void startCall(const QString& calleeUuid) = 0;
  virtual void acceptCall(const QString& callUuid) = 0;
  virtual void hangup(const QString& callUuid) = 0;
  virtual void disconnectFromServer() = 0;
  

 signals:
  void messageSent(const QJsonObject& message);
  void messageReceived(const QJsonObject& message);
  void conversationsLoaded(const QList<Message>& messages);
  void messageError(const QString& error);
  void callStarted(const QString& callUuid, const QString& calleeUuid);
  void callAccepted(const QString& callUuid, const QString& callerUuid);
  void callEnded(const QString& callUuid, const QString& reason);
  void callError(const QString& error);
  void connectionUpdate(ServerType server, bool status);


  
};

#endif
