#include "ChatService_test.h"
#include "Chat/ChatService.h"
#include "Core/ServiceLocator.h"
#include "TestDoubles.h"
#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

void ChatServiceTest::delegatesMessageOperations() {
  FakeMessageService messages; FakeCallService calls; ChatService service(&messages, &calls);
  const Message message("message-1", "sender", "receiver", "message", "Hello", QDateTime::currentDateTimeUtc());
  service.sendMessage(message);
  service.loadConversationsFromDatabaseAndServer();
  QCOMPARE(messages.sendCalls, 1);
  QCOMPARE(messages.loadCalls, 1);
  QCOMPARE(messages.sentMessage.uuid(), QStringLiteral("message-1"));
}

void ChatServiceTest::delegatesCallOperations() {
  FakeMessageService messages; FakeCallService calls; ChatService service(&messages, &calls);
  service.startCall("user-2", "Bob");
  service.hangup();
  service.acceptIncomingCall("Bob");
  service.rejectIncomingCall();
  service.cameraEnabledChanged(true);
  QCOMPARE(calls.startCalls, 1);
  QCOMPARE(calls.hangupCalls, 1);
  QCOMPARE(calls.acceptCalls, 1);
  QCOMPARE(calls.rejectCalls, 1);
  QCOMPARE(calls.cameraCalls, 1);
  QCOMPARE(calls.callUuid, QStringLiteral("user-2"));
  QCOMPARE(calls.callUsername, QStringLiteral("Bob"));
  QCOMPARE(calls.acceptedUsername, QStringLiteral("Bob"));
  QCOMPARE(calls.cameraEnabled, true);
}

void ChatServiceTest::forwardsSignals() {
  FakeMessageService messages; FakeCallService calls; ChatService service(&messages, &calls);
  QSignalSpy messageErrorSpy(&service, &IChatService::messageError);
  QSignalSpy messageReceivedSpy(&service, &IChatService::messageReceived);
  QSignalSpy conversationsSpy(&service, &IChatService::conversationsLoaded);
  QSignalSpy callErrorSpy(&service, &IChatService::callError);
  QSignalSpy incomingSpy(&service, &IChatService::incomingCallReceived);
  QSignalSpy cancelledSpy(&service, &IChatService::incomingCallCancelled);
  QSignalSpy openSpy(&service, &IChatService::openCallWindow);
  QSignalSpy closeSpy(&service, &IChatService::closeCallWindow);
  QSignalSpy connectedSpy(&service, &IChatService::isContactConnectedChanged);
  QSignalSpy cameraSpy(&service, &IChatService::onCameraEnabledChanged);
  const Message message("message-1", "sender", "receiver", "message", "Hello", QDateTime::currentDateTimeUtc());
  const QList<Message> conversations{message};
  emit messages.messageError("message error");
  emit messages.messageReceived(message);
  emit messages.conversationsLoaded(conversations);
  emit calls.callError("call error");
  emit calls.incomingCallReceived("caller-1");
  emit calls.incomingCallCancelled("caller-1");
  emit calls.openCallWindow("Bob");
  emit calls.closeCallWindow();
  emit calls.isContactConnectedChanged(true);
  emit calls.onCameraEnabledChanged(true);
  QCOMPARE(messageErrorSpy.count(), 1);
  QCOMPARE(messageReceivedSpy.count(), 1);
  QCOMPARE(conversationsSpy.count(), 1);
  QCOMPARE(callErrorSpy.count(), 1);
  QCOMPARE(incomingSpy.count(), 1);
  QCOMPARE(cancelledSpy.count(), 1);
  QCOMPARE(openSpy.count(), 1);
  QCOMPARE(closeSpy.count(), 1);
  QCOMPARE(connectedSpy.count(), 1);
  QCOMPARE(cameraSpy.count(), 1);
  QCOMPARE(messageErrorSpy.at(0).at(0).toString(), QStringLiteral("message error"));
  QCOMPARE(messageReceivedSpy.at(0).at(0).value<Message>().uuid(), QStringLiteral("message-1"));
  QCOMPARE(conversationsSpy.at(0).at(0).value<QList<Message>>().size(), 1);
  QCOMPARE(callErrorSpy.at(0).at(0).toString(), QStringLiteral("call error"));
  QCOMPARE(incomingSpy.at(0).at(0).toString(), QStringLiteral("caller-1"));
  QCOMPARE(cancelledSpy.at(0).at(0).toString(), QStringLiteral("caller-1"));
  QCOMPARE(openSpy.at(0).at(0).toString(), QStringLiteral("Bob"));
  QCOMPARE(connectedSpy.at(0).at(0).toBool(), true);
  QCOMPARE(cameraSpy.at(0).at(0).toBool(), true);
}

void ChatServiceTest::disconnectsBothServices() {
  FakeMessageService messages; FakeCallService calls; ChatService service(&messages, &calls);
  service.disconnectFromServer();
  QCOMPARE(messages.disconnectCalls, 1);
  QCOMPARE(calls.disconnectCalls, 1);
}

void ChatServiceTest::missingMessageServiceEmitsErrors() {
  ServiceLocator::instance().clear();
  FakeCallService calls;
  ChatService service(nullptr, &calls);
  QSignalSpy messageErrorSpy(&service, &IChatService::messageError);

  service.sendMessage(Message());
  service.loadConversationsFromDatabaseAndServer();
  service.disconnectFromServer();

  QCOMPARE(messageErrorSpy.count(), 3);
  QCOMPARE(messageErrorSpy.at(0).at(0).toString(), QStringLiteral("Service de messagerie indisponible"));
  QCOMPARE(messageErrorSpy.at(1).at(0).toString(), QStringLiteral("Service de messagerie indisponible"));
  QCOMPARE(messageErrorSpy.at(2).at(0).toString(), QStringLiteral("Service de messagerie indisponible"));
  QCOMPARE(calls.disconnectCalls, 0);
  ServiceLocator::instance().clear();
}

void ChatServiceTest::missingCallServiceEmitsErrors() {
  ServiceLocator::instance().clear();
  FakeMessageService messages;
  ChatService service(&messages, nullptr);
  QSignalSpy callErrorSpy(&service, &IChatService::callError);

  service.startCall("user-2", "Bob");
  service.hangup();
  service.acceptIncomingCall("Bob");
  service.rejectIncomingCall();
  service.cameraEnabledChanged(true);
  service.disconnectFromServer();

  QCOMPARE(callErrorSpy.count(), 6);
  for (const auto& signal : callErrorSpy) {
    QCOMPARE(signal.at(0).toString(), QStringLiteral("Service d'appel indisponible"));
  }
  QCOMPARE(messages.disconnectCalls, 0);
  ServiceLocator::instance().clear();
}