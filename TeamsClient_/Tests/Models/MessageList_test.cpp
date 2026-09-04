#include "MessageList_test.h"

#include "Models/MessageList.h"

#include <QSignalSpy>
#include <QTest>

namespace {
Message makeMessage(const QString& uuid, const QString& content = "Hello") {
  return Message(uuid, "sender-1", "receiver-1", "message", content,
                 QDateTime::fromString("2026-09-03T12:34:56Z", Qt::ISODate));
}
}

void MessageListTest::startsEmpty() {
  const MessageList messages;

  QCOMPARE(messages.rowCount(), 0);
  QVERIFY(!messages.lastMessage().isValid());
  QVERIFY(!messages.data(QModelIndex(), MessageList::ContentRole).isValid());
  QVERIFY(!messages.data(messages.index(0, 0), MessageList::ContentRole).isValid());
}

void MessageListTest::addMessageExposesRolesAndUpdatesLastMessage() {
  MessageList messages;
  const Message first = makeMessage("message-1", "First");
  const Message second = makeMessage("message-2", "Second");
  QSignalSpy rowsInsertedSpy(&messages, &QAbstractItemModel::rowsInserted);

  messages.addMessage(first);
  messages.addMessage(second);

  QCOMPARE(messages.rowCount(), 2);
  QCOMPARE(rowsInsertedSpy.count(), 2);
  const QModelIndex index = messages.index(1, 0);
  QCOMPARE(messages.data(index, MessageList::UuidRole).toString(), second.uuid());
  QCOMPARE(messages.data(index, MessageList::SenderUuidRole).toString(), second.senderUuid());
  QCOMPARE(messages.data(index, MessageList::ContentRole).toString(), second.content());
  QCOMPARE(messages.data(index, MessageList::TimestampRole).toDateTime(), second.timestamp());
  QCOMPARE(messages.data(index, MessageList::FromMeRole).toBool(), second.fromMe());
  QCOMPARE(messages.lastMessage().uuid(), QStringLiteral("message-2"));
}

void MessageListTest::removeMessageUpdatesRows() {
  MessageList messages;
  const Message first = makeMessage("message-1");
  const Message second = makeMessage("message-2");
  messages.addMessage(first);
  messages.addMessage(second);
  QSignalSpy rowsRemovedSpy(&messages, &QAbstractItemModel::rowsRemoved);

  messages.removeMessage(makeMessage("missing"));
  QCOMPARE(rowsRemovedSpy.count(), 0);
  messages.removeMessage(first);

  QCOMPARE(messages.rowCount(), 1);
  QCOMPARE(rowsRemovedSpy.count(), 1);
  QCOMPARE(messages.lastMessage().uuid(), QStringLiteral("message-2"));
}