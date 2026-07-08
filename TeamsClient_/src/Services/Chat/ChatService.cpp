#include "ChatService.h"

#include "Call/CallService.h"
#include "Interfaces/IChatService.h"
#include "Interfaces/IContactService.h"
#include "ServiceLocator.h"

#include <QDebug>
#include <QtCore/qcoreapplication.h>

ChatService::ChatService(IMessageService *messageService, ICallService *callService, QObject *parent)
    : IChatService(parent),
      messageService_(messageService ? messageService : ServiceLocator::instance().getService<IMessageService>()),
      callService_(callService ? callService : ServiceLocator::instance().getService<ICallService>()) {
  Q_ASSERT(messageService_);
  Q_ASSERT(callService_);

  connect(messageService_, &IMessageService::messageError, this, &IChatService::messageError);
  connect(messageService_, &IMessageService::messageReceived, this, &IChatService::messageReceived);
  connect(messageService_, &IMessageService::conversationsLoaded, this, &IChatService::conversationsLoaded);

  connect(callService_, &ICallService::callError, this, &IChatService::callError);
  connect(callService_, &ICallService::incomingCallReceived, this, &IChatService::incomingCallReceived);
  connect(callService_, &ICallService::incomingCallCancelled, this, &IChatService::incomingCallCancelled);
  connect(callService_, &ICallService::openCallWindow, this, &IChatService::openCallWindow);
  connect(callService_, &ICallService::closeCallWindow, this, &IChatService::closeCallWindow);
  connect(callService_, &ICallService::isContactConnectedChanged, this, &IChatService::isContactConnectedChanged);
  connect(callService_, &ICallService::onCameraEnabledChanged, this, &IChatService::onCameraEnabledChanged);
}

void ChatService::sendMessage(const Message &message) {
  if (!messageService_) {
    emit messageError("Service de messagerie indisponible");
    return;
  }
  messageService_->sendMessage(message);
}

void ChatService::loadConversationsFromDatabaseAndServer() {
  if (!messageService_) {
    emit messageError("Service de messagerie indisponible");
    return;
  }
  messageService_->loadConversationsFromServer();
}

void ChatService::startCall(const QString &contactUuid, const QString &contactUsername) {
  if (!callService_) {
    emit callError("Service d'appel indisponible");
    return;
  }
  callService_->startCall(contactUuid, contactUsername);
}

void ChatService::hangup() {
  if (!callService_) {
    emit callError("Service d'appel indisponible");
    return;
  }
  callService_->hangup();
}

void ChatService::acceptIncomingCall(const QString &remoteUsername) {
  if (!callService_) {
    emit callError("Service d'appel indisponible");
    return;
  }
  callService_->acceptCall(remoteUsername);
}

void ChatService::rejectIncomingCall() {
  if (!callService_) {
    emit callError("Service d'appel indisponible");
    return;
  }
  callService_->rejectCall();
}

void ChatService::cameraEnabledChanged(bool cameraEnabled) {
  if (!callService_) {
    emit callError("Service d'appel indisponible");
    return;
  }
  callService_->cameraEnabledChanged(cameraEnabled);
}

void ChatService::disconnectFromServer() {
  if (!messageService_ || !callService_) {
    emit messageError("Service de messagerie indisponible");
    emit callError("Service d'appel indisponible");
  }
  messageService_->disconnectFromServer();
  callService_->disconnectFromServer();
}
