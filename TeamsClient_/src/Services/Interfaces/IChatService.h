#ifndef ICHATSERVICE_H
#define ICHATSERVICE_H

#include "Message.h"
#include "SessionEnum.h"
#include "User.h"
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>

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
  explicit IChatService(QObject *parent = nullptr) : QObject(parent) {};
  virtual ~IChatService() = default;

  virtual void disconnectFromServer() = 0;



  // ---------- Message Service ----------
public:
  virtual void sendMessage(const Message &message) = 0;
  virtual void loadConversationsFromDatabaseAndServer() = 0;

signals:

  void messageError(const QString &error);
  
  void messageReceived(const Message &message);
  void conversationsLoaded(const QList<Message> &messages);



  // ---------- Call Service ----------
public:
  virtual void startCall(const QString &contactUuid, const QString &contactUsername) = 0;
  virtual void hangup() = 0;
  virtual void acceptIncomingCall(const QString &remoteUsername) = 0;
  virtual void rejectIncomingCall() = 0;
  virtual void cameraEnabledChanged(bool cameraEnabled) = 0;

signals:

  void callError(const QString &error);

  void incomingCallReceived(const QString &callerUuid);
  void incomingCallCancelled(const QString &callerUuid);

  void openCallWindow(const QString &remoteUsername);
  void closeCallWindow();

  void isContactConnectedChanged(bool isConnected);
  void onCameraEnabledChanged(bool cameraEnabled);
};

#endif
