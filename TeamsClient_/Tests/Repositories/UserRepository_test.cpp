#include "UserRepository_test.h"

#include "Models/User.h"
#include "Repositories/UserRepository.h"

#include <QDateTime>
#include <QtTest/QtTest>

namespace {
User makeUser(const QString& uuid, bool isMe = false) {
  User user(uuid + "@example.com", "Alice", "Martin", "Online", isMe, "token-1", uuid, "avatar.png",
            "Hello");
  user.setLastReadAt(QDateTime::fromString("2026-09-03T12:34:56+02:00", Qt::ISODate));
  return user;
}
}

void UserRepositoryTest::saveAndFindUserRoundTrip() {
  UserRepository repository;
  const User expected = makeUser("repo-user-roundtrip");
  repository.remove(expected.uuid());

  QVERIFY(repository.save(expected));
  const auto actual = repository.findByUUID(expected.uuid());

  QVERIFY(actual.has_value());
  QCOMPARE(actual->uuid(), expected.uuid());
  QCOMPARE(actual->email(), expected.email());
  QCOMPARE(actual->fullName(), expected.fullName());
  QCOMPARE(actual->status(), expected.status());
  QCOMPARE(actual->token(), expected.token());
  QCOMPARE(actual->avatar(), expected.avatar());
  QCOMPARE(actual->lastReadAt(), expected.lastReadAt());
  QVERIFY(repository.remove(expected.uuid()));
}

void UserRepositoryTest::saveUsesUuidAsUpsertKey() {
  UserRepository repository;
  const QString uuid = QStringLiteral("repo-user-upsert");
  repository.remove(uuid);
  User original = makeUser(uuid);
  User updated = original;
  updated.setStatus("Offline");
  updated.setToken("updated-token");

  QVERIFY(repository.save(original));
  QVERIFY(repository.save(updated));
  const auto actual = repository.findByUUID(uuid);

  QVERIFY(actual.has_value());
  QCOMPARE(actual->status(), QStringLiteral("Offline"));
  QCOMPARE(actual->token(), QStringLiteral("updated-token"));
  QCOMPARE(repository.findAll().count(updated), 1);
  QVERIFY(repository.remove(uuid));
}

void UserRepositoryTest::findAllReturnsStoredUsers() {
  UserRepository repository;
  const User first = makeUser("repo-user-all-1");
  const User second = makeUser("repo-user-all-2");
  repository.remove(first.uuid());
  repository.remove(second.uuid());
  QVERIFY(repository.save(first));
  QVERIFY(repository.save(second));

  const QList<User> users = repository.findAll();

  QCOMPARE(users.count(first), 1);
  QCOMPARE(users.count(second), 1);
  QVERIFY(repository.remove(first.uuid()));
  QVERIFY(repository.remove(second.uuid()));
}

void UserRepositoryTest::updateLastReadAtUpdatesOnlyExistingUser() {
  UserRepository repository;
  const User user = makeUser("repo-user-read");
  repository.remove(user.uuid());
  QVERIFY(repository.save(user));
  const QString timestamp = QStringLiteral("2026-09-03T16:00:00Z");

  QVERIFY(repository.updateLastReadAt(user.uuid(), timestamp));
  const auto actual = repository.findByUUID(user.uuid());
  QVERIFY(actual.has_value());
  QCOMPARE(actual->lastReadAt(), QDateTime::fromString(timestamp, Qt::ISODate));
  QVERIFY(!repository.updateLastReadAt("repo-user-missing", timestamp));
  QVERIFY(repository.remove(user.uuid()));
}

void UserRepositoryTest::removeDeletesOnlyRequestedUser() {
  UserRepository repository;
  const User first = makeUser("repo-user-remove-1");
  const User second = makeUser("repo-user-remove-2");
  repository.remove(first.uuid());
  repository.remove(second.uuid());
  QVERIFY(repository.save(first));
  QVERIFY(repository.save(second));

  QVERIFY(repository.remove(first.uuid()));
  QVERIFY(!repository.findByUUID(first.uuid()).has_value());
  QVERIFY(repository.findByUUID(second.uuid()).has_value());
  QVERIFY(!repository.remove(first.uuid()));
  QVERIFY(repository.remove(second.uuid()));
}

void UserRepositoryTest::removeAllContactsKeepsLocalUser() {
  UserRepository repository;
  const User local = makeUser("repo-user-local", true);
  const User contact = makeUser("repo-user-contact", false);
  repository.remove(local.uuid());
  repository.remove(contact.uuid());
  QVERIFY(repository.save(local));
  QVERIFY(repository.save(contact));

  QVERIFY(repository.removeAllContacts());
  QVERIFY(repository.findByUUID(local.uuid()).has_value());
  QVERIFY(!repository.findByUUID(contact.uuid()).has_value());
  QVERIFY(repository.remove(local.uuid()));
}

void UserRepositoryTest::removeAllClearsUsers() {
  UserRepository repository;
  const User first = makeUser("repo-user-clear-1");
  const User second = makeUser("repo-user-clear-2");
  repository.remove(first.uuid());
  repository.remove(second.uuid());
  QVERIFY(repository.save(first));
  QVERIFY(repository.save(second));

  QVERIFY(repository.removeAll());
  QVERIFY(!repository.findByUUID(first.uuid()).has_value());
  QVERIFY(!repository.findByUUID(second.uuid()).has_value());
}