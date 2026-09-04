#include "MessageService_test.h"
#include "Chat/Message/MessageService.h"
#include "Core/State/UserState.h"
#include "TestDoubles.h"
#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

namespace {
Message testMessage(const QString& uuid = "message-1") {
  return Message(uuid, "sender-1", "receiver-1", "message", "Hello",
                 QDateTime::fromString("2026-09-03T12:34:56Z", Qt::ISODate));
}
}

void MessageServiceTest::sendMessagePersistsAndSendsPayload() {
  FakeNetwork network;
  MockMessageRepository repository;
  MessageService service(&network, &repository);
  UserState::instance().saveLocalUser(User("a@b.test", "Alice", "Martin", "Online", true, "token-1", "user-1"));
  service.sendMessage(testMessage());
  QCOMPARE(network.sentPayloads.size(), 1);
  const QJsonObject payload = network.sentPayloads.first();
  QCOMPARE(payload.value("type").toString(), QStringLiteral("send_message"));
  QCOMPARE(payload.value("token").toString(), QStringLiteral("token-1"));
  QCOMPARE(payload.value("message").toObject().value("uuid").toString(), QStringLiteral("message-1"));
  QCOMPARE(repository.saveCalls, 1);
  UserState::instance().deleteLocalUser();
}

void MessageServiceTest::invalidMessageIsNotSent() {
  FakeNetwork network;
  MockMessageRepository repository;
  MessageService service(&network, &repository);
  service.sendMessage(Message());
  QVERIFY(network.sentPayloads.isEmpty());
}

void MessageServiceTest::incomingMessageIsForwarded() {
  FakeNetwork network;
  MockMessageRepository repository;
  MessageService service(&network, &repository);
  QSignalSpy receivedSpy(&service, &IMessageService::messageReceived);
  network.jsonReceived({{"type", "new_message"}, {"data", testMessage("incoming").toJson()}});
  QCOMPARE(receivedSpy.count(), 1);
  QCOMPARE(repository.saveCalls, 1);
}

void MessageServiceTest::serverErrorIsForwarded() {
  FakeNetwork network;
  MockMessageRepository repository;
  MessageService service(&network, &repository);
  QSignalSpy errorSpy(&service, &IMessageService::messageError);
  network.jsonReceived({{"type", "error_response"}, {"error", "server unavailable"}});
  QCOMPARE(errorSpy.count(), 1);
  QCOMPARE(errorSpy.at(0).at(0).toString(), QStringLiteral("server unavailable"));
}

void MessageServiceTest::disconnectsNetwork() {
  FakeNetwork network;
  MockMessageRepository repository;
  MessageService service(&network, &repository);
  service.disconnectFromServer();
  QVERIFY(network.disconnected);
}

void MessageServiceTest::loadConversationsSendsRequest() {
  FakeNetwork network;
  MockMessageRepository repository;
  MessageService service(&network, &repository);
  UserState::instance().saveLocalUser(User("a@b.test", "Alice", "Martin", "Online", true, "token-1", "user-1"));

  service.loadConversationsFromServer();

  QCOMPARE(network.sentPayloads.size(), 1);
  QCOMPARE(network.sentPayloads.first().value("type").toString(), QStringLiteral("load_conversations"));
  QCOMPARE(network.sentPayloads.first().value("token").toString(), QStringLiteral("token-1"));
  UserState::instance().deleteLocalUser();
}

void MessageServiceTest::deleteAllCallsRepository() {
  FakeNetwork network;
  MockMessageRepository repository;
  MessageService service(&network, &repository);

  service.deleteAll();

  QCOMPARE(repository.removeAllCalls, 1);
  QVERIFY(network.sentPayloads.isEmpty());
}

void MessageServiceTest::conversationsLoadedPersistsAndEmits() {
  FakeNetwork network;
  MockMessageRepository repository;
  MessageService service(&network, &repository);
  QSignalSpy loadedSpy(&service, &IMessageService::conversationsLoaded);

  network.jsonReceived({{"type", "conversations_loaded"},
                        {"data", QJsonArray{testMessage("one").toJson(), testMessage("two").toJson()}}});

  QCOMPARE(repository.saveCalls, 2);
  QCOMPARE(repository.findAllCalls, 1);
  QCOMPARE(loadedSpy.count(), 1);
  QCOMPARE(loadedSpy.at(0).at(0).value<QList<Message>>().size(), 2);
}

void MessageServiceTest::malformedServerResponseEmitsError() {
  FakeNetwork network;
  MockMessageRepository repository;
  MessageService service(&network, &repository);
  QSignalSpy errorSpy(&service, &IMessageService::messageError);

  network.jsonReceived({{"type", "conversations_loaded"}, {"data", QJsonObject{}}});

  QCOMPARE(errorSpy.count(), 1);
  QCOMPARE(errorSpy.at(0).at(0).toString(), QStringLiteral("[MessageService] Missing data field in server response"));
}