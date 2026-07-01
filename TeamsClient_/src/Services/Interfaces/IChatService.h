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
  virtual void sendMessage(const Message& message) = 0;
  virtual void loadConversationsFromDatabaseAndServer() = 0;
  virtual void startCall(const QString &contactUuid, const QString &contactUsername) = 0;
  virtual void hangup() = 0;
  virtual void disconnectFromServer() = 0;
  
  virtual void acceptIncomingCall() = 0;
  virtual void rejectIncomingCall() = 0;


 signals:
  // void messageSent(const QJsonObject& message); //Pas aencore utilisé
  void messageReceived(const Message& message);
  void messageError(const QString& error);
  void conversationsLoaded(const QList<Message>& messages);

  void incomingCallReceived(const QString &callerUuid);

  void callReceived(const QString &callUuid, const QString& callerUuid);
  void callStarted(const QString &callUuid);
  void callAccepted(const QString& callUuid, const QString& callerUuid);
  void callEnded(const QString& callUuid, const QString& reason);
  void callError(const QString& error);

  void openCallWindow(const QString &remoteUsername);
  void closeCallWindow();
  
  void connectionUpdate(ServerType server, bool status);


  
};

#endif
