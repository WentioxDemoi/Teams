#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include "Interfaces/ICallService.h"
#include "Interfaces/IChatService.h"
#include "Interfaces/IContactService.h"
#include "Interfaces/IMessageService.h"
#include "Message.h"
#include <QObject>
#include <QtCore/qcoreapplication.h>

/**
 * @class ChatService
 * @brief Façade de plus haut niveau qui regroupe les services de chat.
 *
 * Délègue les appels à MessageService et CallService tout en exposant une API
 * unique aux ViewModels.
 */
class ChatService : public IChatService {
  Q_OBJECT
public:
  explicit ChatService(IMessageService *messageService = nullptr, ICallService *callService = nullptr,
                       QObject *parent = nullptr);

  // ---------- Message Service ----------
  void sendMessage(const Message &message) override;
  void loadConversationsFromDatabaseAndServer() override;

  // ---------- Call Service ----------
  void startCall(const QString &contactUuid, const QString &contactUsername) override;
  void hangup() override;

  void acceptIncomingCall(const QString &remoteUsername) override;
  void rejectIncomingCall() override;

  void cameraEnabledChanged(bool cameraEnabled) override;

  void disconnectFromServer() override;

private:
  IMessageService *messageService_;
  ICallService *callService_;
};

#endif
