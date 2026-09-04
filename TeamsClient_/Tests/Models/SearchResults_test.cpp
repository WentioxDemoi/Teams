#include "SearchResults_test.h"

#include "Models/SearchResults.h"

#include <QSignalSpy>
#include <QTest>

namespace {
User makeUser(const QString& uuid, const QString& status = "Offline") {
  return User(uuid + "@example.com", "First", uuid, status, false, {}, uuid, "avatar.png",
              "Last message");
}
}

void SearchResultsTest::startsEmptyAndFindReturnsInvalidUser() {
  const SearchResults results;

  QCOMPARE(results.rowCount(), 0);
  QVERIFY(!results.findByUuid("missing").isValid());
  QVERIFY(results.get(-1).isEmpty());
  QVERIFY(results.get(0).isEmpty());
  QVERIFY(!results.data(QModelIndex(), SearchResults::UuidRole).isValid());
}

void SearchResultsTest::addUserExposesRolesAndGetMap() {
  SearchResults results;
  const User user = makeUser("user-1", "Online");
  QSignalSpy rowsInsertedSpy(&results, &QAbstractItemModel::rowsInserted);

  results.addUser(user);

  QCOMPARE(results.rowCount(), 1);
  QCOMPARE(rowsInsertedSpy.count(), 1);
  const QModelIndex index = results.index(0, 0);
  QCOMPARE(results.data(index, SearchResults::UsernameRole).toString(), user.fullName());
  QCOMPARE(results.data(index, SearchResults::StatusRole).toString(), user.status());
  QCOMPARE(results.data(index, SearchResults::AvatarRole).toString(), user.avatar());
  QCOMPARE(results.data(index, SearchResults::LastMessageRole).toString(), user.lastMessage());
  QCOMPARE(results.data(index, SearchResults::InitialsRole).toString(), user.initials());
  QCOMPARE(results.data(index, SearchResults::AvatarColorRole).toString(), user.avatarColor());
  QCOMPARE(results.data(index, SearchResults::OnlineRole).toBool(), true);
  QCOMPARE(results.data(index, SearchResults::UuidRole).toString(), user.uuid());
  QCOMPARE(results.get(0).value("uuid").toString(), user.uuid());
  QCOMPARE(results.get(0).value("username").toString(), user.fullName());
  QCOMPARE(results.findByUuid(user.uuid()).uuid(), user.uuid());
}

void SearchResultsTest::setUsersReplacesContents() {
  SearchResults results;
  results.addUser(makeUser("old"));
  const QList<User> users{makeUser("user-1"), makeUser("user-2", "Online")};
  QSignalSpy resetSpy(&results, &QAbstractItemModel::modelReset);

  results.setUsers(users);

  QCOMPARE(resetSpy.count(), 1);
  QCOMPARE(results.rowCount(), 2);
  QCOMPARE(results.getSearchResultsList().at(0).uuid(), QStringLiteral("user-1"));
  QCOMPARE(results.getSearchResultsList().at(1).uuid(), QStringLiteral("user-2"));
}

void SearchResultsTest::removeAndClearUpdateModel() {
  SearchResults results;
  const User first = makeUser("user-1");
  const User second = makeUser("user-2");
  results.setUsers({first, second});
  QSignalSpy rowsRemovedSpy(&results, &QAbstractItemModel::rowsRemoved);
  QSignalSpy resetSpy(&results, &QAbstractItemModel::modelReset);

  results.removeUser(makeUser("missing"));
  QCOMPARE(rowsRemovedSpy.count(), 0);
  results.removeUser(first);
  QCOMPARE(rowsRemovedSpy.count(), 1);
  QCOMPARE(results.rowCount(), 1);

  results.clear();
  QCOMPARE(resetSpy.count(), 1);
  QCOMPARE(results.rowCount(), 0);
  results.clear();
  QCOMPARE(resetSpy.count(), 1);
}

void SearchResultsTest::searchLoadedReplacesResults() {
  SearchResults results;
  results.addUser(makeUser("old"));
  QSignalSpy resetSpy(&results, &QAbstractItemModel::modelReset);

  results.onUsersSearchLoaded({makeUser("user-1"), makeUser("user-2")});

  QCOMPARE(resetSpy.count(), 2);
  QCOMPARE(results.rowCount(), 2);
  QCOMPARE(results.findByUuid("old").isValid(), false);
  QCOMPARE(results.findByUuid("user-2").uuid(), QStringLiteral("user-2"));
}