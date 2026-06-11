#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <QObject>
#include "Interfaces/IChatService.h"
#include "Interfaces/IMessageService.h"
#include "Interfaces/IContactService.h"
#include "Interfaces/ICallService.h"

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
  explicit ChatService(IMessageService* messageService = nullptr,
                       IContactService* contactService = nullptr,
                       ICallService* callService = nullptr,
                       QObject* parent = nullptr);

 public slots:
  void sendMessage(const QString& recipientUuid, const QString& content) override;
  void loadConversation(const QString& userUuid) override;
  void loadContacts() override;
  void startCall(const QString& calleeUuid) override;
  void acceptCall(const QString& callUuid) override;
  void hangup(const QString& callUuid) override;
  void disconnectFromServer() override;

 private:
  IMessageService* messageService_;
  IContactService* contactService_;
  ICallService* callService_;
};

#endif
