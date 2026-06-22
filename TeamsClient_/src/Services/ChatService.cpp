#include "ChatService.h"

#include "Call/CallService.h"
#include "Contact/ContactService.h"
#include "Interfaces/IChatService.h"
#include "Interfaces/IContactService.h"
#include "ServiceLocator.h"

#include <QDebug>
#include <QtCore/qcoreapplication.h>

ChatService::ChatService(IMessageService* messageService,
                         IContactService* contactService,
                         ICallService* callService,
                         QObject* parent)
    : IChatService(parent),
      messageService_(messageService ? messageService : ServiceLocator::instance().getService<IMessageService>()),
      contactService_(contactService ? contactService : ServiceLocator::instance().getService<IContactService>()),
      callService_(callService ? callService : ServiceLocator::instance().getService<ICallService>()) {
  Q_ASSERT(messageService_);
  Q_ASSERT(contactService_);
  Q_ASSERT(callService_);

  connect(messageService_, &IMessageService::messageSent, this, &IChatService::messageSent);
  connect(messageService_, &IMessageService::messageReceived, this, &IChatService::messageReceived);
  connect(messageService_, &IMessageService::conversationsLoaded, this, &IChatService::conversationsLoaded);
  connect(messageService_, &IMessageService::messageError, this, &IChatService::messageError);
  connect(messageService_, &IMessageService::connectionUpdate, this, &IChatService::connectionUpdate);

  connect(contactService_, &IContactService::contactsLoaded, this, &IChatService::contactsLoaded);
  connect(contactService_, &IContactService::contactError, this, &IChatService::messageError);
  connect(contactService_, &IContactService::connectionUpdate, this, &IChatService::connectionUpdate);
  connect(contactService_, &IContactService::contactsSearchLoaded, this, &IChatService::contactsSearchLoaded);
  // Connect saveContact (DB Locale + server)

  connect(callService_, &ICallService::callStarted, this, &IChatService::callStarted);
  connect(callService_, &ICallService::callAccepted, this, &IChatService::callAccepted);
  connect(callService_, &ICallService::callEnded, this, &IChatService::callEnded);
  connect(callService_, &ICallService::callError, this, &IChatService::callError);
  connect(callService_, &ICallService::connectionUpdate, this, &IChatService::connectionUpdate);
}

void ChatService::sendMessage(const QString& recipientUuid, const QString& content) {
  if (!messageService_) {
    emit messageError("Service de messagerie indisponible");
    return;
  }
  messageService_->sendMessage(recipientUuid, content);
}

void ChatService::saveContact

void ChatService::loadConversationsFromDatabaseAndServer() {
  if (!messageService_) {
    emit messageError("Service de messagerie indisponible");
    return;
  }
  messageService_->loadConversationsFromDatabaseAndServer();
}

void ChatService::loadContactsFromDatabaseAndServer() {
  if (!contactService_) {
    emit messageError("Service de contacts indisponible");
    return;
  }
  contactService_->loadContactsFromDatabaseAndServer();
}

void ChatService::startCall(const QString& calleeUuid) {
  if (!callService_) {
    emit callError("Service d'appel indisponible");
    return;
  }
  callService_->startCall(calleeUuid);
}

void ChatService::acceptCall(const QString& callUuid) {
  if (!callService_) {
    emit callError("Service d'appel indisponible");
    return;
  }
  callService_->acceptCall(callUuid);
}

void ChatService::hangup(const QString& callUuid) {
  if (!callService_) {
    emit callError("Service d'appel indisponible");
    return;
  }
  callService_->hangup(callUuid);
}

void ChatService::disconnectFromServer() {
  if (messageService_) {
    messageService_->disconnectFromServer();
  }
  if (contactService_) {
    contactService_->disconnectFromServer();
  }
  if (callService_) {
    callService_->disconnectFromServer();
  }
}

void ChatService::searchContacts(const QString &query)
{
  contactService_->searchContacts(query);
}