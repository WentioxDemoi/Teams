#include "ContactList_test.h"

#include "Models/ContactList.h"

#include <QSignalSpy>
#include <QTest>

namespace {
User makeUser(const QString &uuid, const QString &status = "Offline") {
  return User(uuid + "@example.com", "First", uuid, status, false, {}, uuid, "avatar.png",
              "Last message");
}
}

void ContactListTest::startsEmptyAndRejectsInvalidIndexes() {
  const ContactList contacts;

  QCOMPARE(contacts.rowCount(), 0);
  QVERIFY(contacts.get(-1).isEmpty());
  QVERIFY(contacts.get(0).isEmpty());
  QVERIFY(!contacts.data(QModelIndex(), ContactList::UuidRole).isValid());
  QVERIFY(!contacts.data(contacts.index(0, 0), ContactList::UuidRole).isValid());
}

void ContactListTest::addUserExposesExpectedRoles() {
  ContactList contacts;
  const User user = makeUser("user-1", "Online");

  QSignalSpy rowsInsertedSpy(&contacts, &QAbstractItemModel::rowsInserted);
  contacts.addUser(user);

  QCOMPARE(contacts.rowCount(), 1);
  QCOMPARE(rowsInsertedSpy.count(), 1);
  const QModelIndex index = contacts.index(0, 0);
  QCOMPARE(contacts.data(index, ContactList::UsernameRole).toString(), user.fullName());
  QCOMPARE(contacts.data(index, ContactList::StatusRole).toString(), QStringLiteral("Online"));
  QCOMPARE(contacts.data(index, ContactList::AvatarRole).toString(), user.avatar());
  QCOMPARE(contacts.data(index, ContactList::LastMessageRole).toString(), user.lastMessage());
  QCOMPARE(contacts.data(index, ContactList::InitialsRole).toString(), user.initials());
  QCOMPARE(contacts.data(index, ContactList::AvatarColorRole).toString(), user.avatarColor());
  QCOMPARE(contacts.data(index, ContactList::OnlineRole).toBool(), true);
  QCOMPARE(contacts.data(index, ContactList::UuidRole).toString(), user.uuid());
  QCOMPARE(contacts.data(index, ContactList::UnreadRole).toInt(), 0);
}

void ContactListTest::removeUserUpdatesRows() {
  ContactList contacts;
  const User first = makeUser("user-1");
  const User second = makeUser("user-2");
  contacts.addUser(first);
  contacts.addUser(second);

  QSignalSpy rowsRemovedSpy(&contacts, &QAbstractItemModel::rowsRemoved);
  contacts.removeUser(first);
  contacts.removeUser(makeUser("missing"));

  QCOMPARE(contacts.rowCount(), 1);
  QCOMPARE(rowsRemovedSpy.count(), 1);
  QCOMPARE(contacts.get(0).value("uuid").toString(), QStringLiteral("user-2"));
}

void ContactListTest::updateMethodsChangeDataAndEmitRoles() {
  ContactList contacts;
  contacts.addUser(makeUser("user-1"));
  const QModelIndex index = contacts.index(0, 0);

  QSignalSpy changedSpy(&contacts, &QAbstractItemModel::dataChanged);
  contacts.updateLastMessage("user-1", "New message");
  QCOMPARE(contacts.data(index, ContactList::LastMessageRole).toString(), QStringLiteral("New message"));
  QCOMPARE(changedSpy.count(), 1);
  QCOMPARE(changedSpy.at(0).at(2).value<QVector<int>>(), QVector<int>{ContactList::LastMessageRole});

  changedSpy.clear();
  contacts.setUnreadCount("user-1", 3);
  QCOMPARE(contacts.data(index, ContactList::UnreadRole).toInt(), 3);
  QCOMPARE(changedSpy.count(), 1);
  QCOMPARE(changedSpy.at(0).at(2).value<QVector<int>>(), QVector<int>{ContactList::UnreadRole});

  changedSpy.clear();
  contacts.updateStatus("user-1", "Online");
  QCOMPARE(contacts.data(index, ContactList::StatusRole).toString(), QStringLiteral("Online"));
  QCOMPARE(contacts.data(index, ContactList::OnlineRole).toBool(), true);
  QCOMPARE(changedSpy.count(), 1);
  const QVector<int> expectedRoles{ContactList::StatusRole, ContactList::OnlineRole};
  QCOMPARE(changedSpy.at(0).at(2).value<QVector<int>>(), expectedRoles);
}

void ContactListTest::incrementUnreadCountMovesContactToTop() {
  ContactList contacts;
  contacts.addUser(makeUser("user-1"));
  contacts.addUser(makeUser("user-2"));
  contacts.addUser(makeUser("user-3"));

  QSignalSpy rowsMovedSpy(&contacts, &QAbstractItemModel::rowsMoved);
  contacts.incrementUnreadCount("user-3");

  QCOMPARE(contacts.get(0).value("uuid").toString(), QStringLiteral("user-3"));
  QCOMPARE(contacts.get(0).value("unreadCount").toInt(), 0);
  QCOMPARE(rowsMovedSpy.count(), 1);

  contacts.incrementUnreadCount("missing");
  contacts.moveToTop("missing");
  QCOMPARE(contacts.rowCount(), 3);
}

void ContactListTest::lookupReturnsContactOrEmptyMap() {
  ContactList contacts;
  contacts.addUser(makeUser("user-1"));

  QCOMPARE(contacts.findByUuid("user-1").value("uuid").toString(), QStringLiteral("user-1"));
  QVERIFY(contacts.findByUuid("missing").isEmpty());
  QCOMPARE(contacts.toVariantMap(makeUser("user-2")).value("username").toString(),
           QStringLiteral("First user-2"));
}