#include "Message_test.h"

#include "Core/State/UserState.h"
#include "Models/Message.h"

#include <QUuid>
#include <QTest>

void MessageTest::defaultMessageIsInvalid() {
  QVERIFY(!Message().isValid());
}

void MessageTest::validMessageIsRecognized() {
  const QDateTime timestamp = QDateTime::fromString("2026-09-02T12:34:56Z", Qt::ISODate);
  const Message message("message-1", "sender-1", "receiver-1", "message", "Hello", timestamp);

  QVERIFY(message.isValid());
  QCOMPARE(message.uuid(), QStringLiteral("message-1"));
  QCOMPARE(message.senderUuid(), QStringLiteral("sender-1"));
  QCOMPARE(message.receiverUuid(), QStringLiteral("receiver-1"));
  QCOMPARE(message.chatType(), QStringLiteral("message"));
  QCOMPARE(message.content(), QStringLiteral("Hello"));
  QCOMPARE(message.timestamp(), timestamp);
}

void MessageTest::jsonRoundTripPreservesFields() {
  const QDateTime timestamp = QDateTime::fromString("2026-09-02T12:34:56+02:00", Qt::ISODate);
  const Message original("message-1", "sender-1", "receiver-1", "image", "image-data", timestamp);

  const Message restored = Message::fromJson(original.toJson());

  QCOMPARE(restored.uuid(), original.uuid());
  QCOMPARE(restored.senderUuid(), original.senderUuid());
  QCOMPARE(restored.receiverUuid(), original.receiverUuid());
  QCOMPARE(restored.chatType(), original.chatType());
  QCOMPARE(restored.content(), original.content());
  QCOMPARE(restored.timestamp(), original.timestamp().toUTC());
}

void MessageTest::emptyUuidIsGenerated() {
  const Message message({}, "sender-1", "receiver-1", "message", "Hello", QDateTime::currentDateTimeUtc());

  QVERIFY(!message.uuid().isEmpty());
  QVERIFY(!QUuid(message.uuid()).isNull());
}

void MessageTest::outgoingMessageUsesLocalUser() {
  UserState::instance().saveLocalUser(User("alice@example.com", "Alice", "Martin", "Online", true,
                                           "token", "local-user"));

  const Message message = Message::createOutgoing("receiver-1", "message", "Hello");

  QCOMPARE(message.senderUuid(), QStringLiteral("local-user"));
  QCOMPARE(message.receiverUuid(), QStringLiteral("receiver-1"));
  QCOMPARE(message.content(), QStringLiteral("Hello"));
  QVERIFY(message.fromMe());
  QVERIFY(message.isValid());

  UserState::instance().deleteLocalUser();
}