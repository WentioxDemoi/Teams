#include "MessageRepository_test.h"

#include "Models/Message.h"
#include "Repositories/MessageRepository.h"

#include <QDateTime>
#include <QtTest/QtTest>

namespace {
Message makeMessage(const QString& uuid, const QString& sender, const QString& receiver,
                    const QString& content, const QString& timestamp) {
  return Message(uuid, sender, receiver, "message", content,
                 QDateTime::fromString(timestamp, Qt::ISODate));
}
}

void MessageRepositoryTest::saveAndFindMessageRoundTrip() {
  MessageRepository repository;
  const Message expected = makeMessage("repo-message-roundtrip", "sender-1", "receiver-1", "Hello",
                                       "2026-09-03T12:34:56Z");
  repository.remove(expected.uuid());

  QVERIFY(repository.save(expected));
  const auto actual = repository.findByUUID(expected.uuid());

  QVERIFY(actual.has_value());
  QCOMPARE(actual->uuid(), expected.uuid());
  QCOMPARE(actual->senderUuid(), expected.senderUuid());
  QCOMPARE(actual->receiverUuid(), expected.receiverUuid());
  QCOMPARE(actual->chatType(), expected.chatType());
  QCOMPARE(actual->content(), expected.content());
  QCOMPARE(actual->timestamp(), expected.timestamp());
  QVERIFY(repository.remove(expected.uuid()));
}

void MessageRepositoryTest::saveUsesUuidAsUpsertKey() {
  MessageRepository repository;
  const QString uuid = QStringLiteral("repo-message-upsert");
  repository.remove(uuid);
  const Message original = makeMessage(uuid, "sender-1", "receiver-1", "old", "2026-09-03T12:00:00Z");
  const Message updated = makeMessage(uuid, "sender-1", "receiver-1", "new", "2026-09-03T13:00:00Z");

  QVERIFY(repository.save(original));
  QVERIFY(repository.save(updated));
  const auto actual = repository.findByUUID(uuid);

  QVERIFY(actual.has_value());
  QCOMPARE(actual->content(), QStringLiteral("new"));
  QCOMPARE(repository.findAll().count(updated), 1);
  QVERIFY(repository.remove(uuid));
}

void MessageRepositoryTest::findAllReturnsMessagesInTimestampOrder() {
  MessageRepository repository;
  const Message later = makeMessage("repo-message-later", "sender-1", "receiver-1", "later", "2026-09-03T13:00:00Z");
  const Message earlier = makeMessage("repo-message-earlier", "sender-1", "receiver-1", "earlier", "2026-09-03T12:00:00Z");
  repository.remove(later.uuid());
  repository.remove(earlier.uuid());
  QVERIFY(repository.save(later));
  QVERIFY(repository.save(earlier));

  const QList<Message> messages = repository.findAll();
  const int earlierIndex = messages.indexOf(earlier);
  const int laterIndex = messages.indexOf(later);
  QVERIFY(earlierIndex >= 0);
  QVERIFY(laterIndex >= 0);
  QVERIFY(earlierIndex < laterIndex);
  QVERIFY(repository.remove(later.uuid()));
  QVERIFY(repository.remove(earlier.uuid()));
}

void MessageRepositoryTest::findForConversationMatchesBothDirections() {
  MessageRepository repository;
  const Message outgoing = makeMessage("repo-message-conversation-1", "alice", "bob", "outgoing", "2026-09-03T12:00:00Z");
  const Message incoming = makeMessage("repo-message-conversation-2", "bob", "alice", "incoming", "2026-09-03T13:00:00Z");
  const Message unrelated = makeMessage("repo-message-conversation-3", "alice", "carol", "unrelated", "2026-09-03T14:00:00Z");
  repository.remove(outgoing.uuid()); repository.remove(incoming.uuid()); repository.remove(unrelated.uuid());
  QVERIFY(repository.save(outgoing)); QVERIFY(repository.save(incoming)); QVERIFY(repository.save(unrelated));

  const QList<Message> conversation = repository.findForConversation("alice", "bob");

  QCOMPARE(conversation.count(outgoing), 1);
  QCOMPARE(conversation.count(incoming), 1);
  QCOMPARE(conversation.count(unrelated), 0);
  QVERIFY(repository.remove(outgoing.uuid())); QVERIFY(repository.remove(incoming.uuid())); QVERIFY(repository.remove(unrelated.uuid()));
}

void MessageRepositoryTest::findForParticipantMatchesSenderOrReceiver() {
  MessageRepository repository;
  const Message sent = makeMessage("repo-message-participant-1", "alice", "bob", "sent", "2026-09-03T12:00:00Z");
  const Message received = makeMessage("repo-message-participant-2", "carol", "alice", "received", "2026-09-03T13:00:00Z");
  const Message unrelated = makeMessage("repo-message-participant-3", "bob", "carol", "unrelated", "2026-09-03T14:00:00Z");
  repository.remove(sent.uuid()); repository.remove(received.uuid()); repository.remove(unrelated.uuid());
  QVERIFY(repository.save(sent)); QVERIFY(repository.save(received)); QVERIFY(repository.save(unrelated));

  const QList<Message> messages = repository.findForParticipant("alice");

  QCOMPARE(messages.count(sent), 1);
  QCOMPARE(messages.count(received), 1);
  QCOMPARE(messages.count(unrelated), 0);
  QVERIFY(repository.remove(sent.uuid())); QVERIFY(repository.remove(received.uuid())); QVERIFY(repository.remove(unrelated.uuid()));
}

void MessageRepositoryTest::removeDeletesOnlyRequestedMessage() {
  MessageRepository repository;
  const Message first = makeMessage("repo-message-remove-1", "alice", "bob", "first", "2026-09-03T12:00:00Z");
  const Message second = makeMessage("repo-message-remove-2", "alice", "bob", "second", "2026-09-03T13:00:00Z");
  repository.remove(first.uuid()); repository.remove(second.uuid());
  QVERIFY(repository.save(first)); QVERIFY(repository.save(second));

  QVERIFY(repository.remove(first.uuid()));
  QVERIFY(!repository.findByUUID(first.uuid()).has_value());
  QVERIFY(repository.findByUUID(second.uuid()).has_value());
  QVERIFY(!repository.remove(first.uuid()));
  QVERIFY(repository.remove(second.uuid()));
}

void MessageRepositoryTest::removeAllClearsMessages() {
  MessageRepository repository;
  const Message first = makeMessage("repo-message-clear-1", "alice", "bob", "first", "2026-09-03T12:00:00Z");
  const Message second = makeMessage("repo-message-clear-2", "alice", "bob", "second", "2026-09-03T13:00:00Z");
  repository.remove(first.uuid()); repository.remove(second.uuid());
  QVERIFY(repository.save(first)); QVERIFY(repository.save(second));

  QVERIFY(repository.removeAll());
  QVERIFY(!repository.findByUUID(first.uuid()).has_value());
  QVERIFY(!repository.findByUUID(second.uuid()).has_value());
}