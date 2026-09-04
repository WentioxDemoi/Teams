#include "LocalUserService_test.h"

#include "Services/LocalUserService.h"
#include "TestDoubles.h"

#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

void LocalUserServiceTest::saveLocalUserUpdatesStateAndEmitsSignal() {
  UserState state;
  MockUserRepository repository;
  LocalUserService service(&state, &repository);
  const User user("alice@example.com", "Alice", "Martin", "Online", true, "token-1", "user-1");
  QSignalSpy savedSpy(&service, &ILocalUserService::localUserSaved);

  service.saveLocalUser(user);

  QCOMPARE(state.localUser().uuid(), QStringLiteral("user-1"));
  QCOMPARE(state.localUser().token(), QStringLiteral("token-1"));
  QCOMPARE(savedSpy.count(), 1);
  QCOMPARE(savedSpy.at(0).at(0).value<User>().uuid(), QStringLiteral("user-1"));
  QCOMPARE(repository.saveCalls, 0);
}

void LocalUserServiceTest::deleteAllCallsRepository() {
  UserState state;
  MockUserRepository repository;
  LocalUserService service(&state, &repository);

  service.deleteAll();

  QCOMPARE(repository.removeAllCalls, 1);
}