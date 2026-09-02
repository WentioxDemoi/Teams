#include "User_test.h"

#include "Models/User.h"

#include <QJsonObject>
#include <QTest>

void UserTest::defaultUserIsInvalid() {
  const User user;

  QVERIFY(!user.isValid());
  QCOMPARE(user.unreadCount(), 0);
  QVERIFY(user.token().isEmpty());
}

void UserTest::derivedPropertiesAreComputed() {
  const User user("alice@example.com", "Alice", "Martin", "Online", false, "token", "user-1",
                  "avatar.png", "Hello");

  QCOMPARE(user.fullName(), QStringLiteral("Alice Martin"));
  QCOMPARE(user.initials(), QStringLiteral("AM"));
  QVERIFY(user.online());
  QCOMPARE(user.avatar(), QStringLiteral("avatar.png"));
  QCOMPARE(user.lastMessage(), QStringLiteral("Hello"));
}

void UserTest::validityRequiresEmailNameAndUuid() {
  User user("alice@example.com", "Alice", "Martin", "Offline", false, "", "user-1");
  QVERIFY(user.isValid());

  user.setEmail({});
  QVERIFY(!user.isValid());
  user.setEmail("alice@example.com");
  user.setFirstName({});
  QVERIFY(!user.isValid());
  user.setFirstName("Alice");
  user.setUuid({});
  QVERIFY(!user.isValid());
}

void UserTest::jsonRoundTripPreservesSerializedFields() {
  User original("alice@example.com", "Alice", "Martin", "Online", true, "token", "user-1",
                "avatar.png", "Hello");

  const User restored = User::fromJson(original.toJson());

  QCOMPARE(restored.email(), original.email());
  QCOMPARE(restored.firstName(), original.firstName());
  QCOMPARE(restored.lastName(), original.lastName());
  QCOMPARE(restored.status(), original.status());
  QCOMPARE(restored.isMe(), original.isMe());
  QCOMPARE(restored.token(), original.token());
  QCOMPARE(restored.uuid(), original.uuid());
  QCOMPARE(restored.avatar(), original.avatar());
}

void UserTest::fromJsonIgnoresWrongTypes() {
  const User user = User::fromJson({
      {"email", 42},
      {"firstName", "Alice"},
      {"lastName", "Martin"},
      {"status", true},
      {"isMe", "yes"},
      {"uuid", "user-1"},
  });

  QVERIFY(user.email().isEmpty());
  QCOMPARE(user.firstName(), QStringLiteral("Alice"));
  QCOMPARE(user.status(), QString());
  QVERIFY(!user.isMe());
  QCOMPARE(user.uuid(), QStringLiteral("user-1"));
}

void UserTest::avatarColorIsDeterministic() {
  const User first("a@b.test", "A", "B", "Offline", false, "", "same-id");
  const User second("c@d.test", "C", "D", "Offline", false, "", "same-id");

  QCOMPARE(first.avatarColor(), second.avatarColor());
  QVERIFY(first.avatarColor().startsWith('#'));
  QCOMPARE(first.avatarColor().size(), 7);
}