#include "ContactService_test.h"

#include "Core/State/UserState.h"
#include "Services/Contact/ContactService.h"
#include "TestDoubles.h"

#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

namespace {
User contactUser(const QString& uuid = "user-1", const QString& status = "Offline") {
  return User(uuid + "@example.com", "Alice", "Martin", status, false, {}, uuid, "avatar.png");
}
}

void ContactServiceTest::searchUsersSendsRequestAndIgnoresBlankQuery() {
  FakeNetwork network;
  MockUserRepository repository;
  ContactService service(&network, &repository);
  UserState::instance().saveLocalUser(User("me@example.com", "Me", "User", "Online", true, "token-1", "me"));

  service.searchUsers("   ");
  QCOMPARE(network.sentPayloads.size(), 0);
  service.searchUsers("alice");

  QCOMPARE(network.sentPayloads.size(), 1);
  const QJsonObject payload = network.sentPayloads.first();
  QCOMPARE(payload.value("type").toString(), QStringLiteral("search_users"));
  QCOMPARE(payload.value("query").toString(), QStringLiteral("alice"));
  QCOMPARE(payload.value("token").toString(), QStringLiteral("token-1"));
  UserState::instance().deleteLocalUser();
}

void ContactServiceTest::resolveUserSendsRequest() {
  FakeNetwork network;
  MockUserRepository repository;
  ContactService service(&network, &repository);

  service.resolveUserByUuid("user-2");

  QCOMPARE(network.sentPayloads.size(), 1);
  QCOMPARE(network.sentPayloads.first().value("type").toString(), QStringLiteral("resolve_user_by_uuid"));
  QCOMPARE(network.sentPayloads.first().value("contactUuid").toString(), QStringLiteral("user-2"));
}

void ContactServiceTest::saveContactPersistsAndSendsRequest() {
  FakeNetwork network;
  MockUserRepository repository;
  ContactService service(&network, &repository);
  const User user = contactUser();

  service.saveContact(user);

  QCOMPARE(repository.saveCalls, 1);
  QCOMPARE(repository.savedUsers.first().uuid(), QStringLiteral("user-1"));
  QCOMPARE(network.sentPayloads.size(), 1);
  QCOMPARE(network.sentPayloads.first().value("type").toString(), QStringLiteral("add_contact"));
  QCOMPARE(network.sentPayloads.first().value("contactUuid").toString(), QStringLiteral("user-1"));
}

void ContactServiceTest::saveContactFailureEmitsError() {
  FakeNetwork network;
  MockUserRepository repository;
  repository.saveResult = false;
  ContactService service(&network, &repository);
  QSignalSpy errorSpy(&service, &IContactService::contactError);

  service.saveContact(contactUser());

  QCOMPARE(repository.saveCalls, 1);
  QVERIFY(network.sentPayloads.isEmpty());
  QCOMPARE(errorSpy.count(), 1);
  QCOMPARE(errorSpy.at(0).at(0).toString(), QStringLiteral("Impossible de sauvegarder le contact"));
}

void ContactServiceTest::updateLastReadAtPersistsBeforeSending() {
  FakeNetwork network;
  MockUserRepository repository;
  ContactService service(&network, &repository);

  service.updateLastReadAt("user-1");

  QCOMPARE(repository.updateLastReadAtCalls, 1);
  QCOMPARE(repository.updatedUuid, QStringLiteral("user-1"));
  QVERIFY(!repository.updatedTimestamp.isEmpty());
  QCOMPARE(network.sentPayloads.size(), 1);
  QCOMPARE(network.sentPayloads.first().value("type").toString(), QStringLiteral("update_last_read_at"));
  QCOMPARE(network.sentPayloads.first().value("contactUuid").toString(), QStringLiteral("user-1"));
  QCOMPARE(network.sentPayloads.first().value("lastReadAt").toString(), repository.updatedTimestamp);
}

void ContactServiceTest::updateLastReadAtFailureDoesNotSend() {
  FakeNetwork network;
  MockUserRepository repository;
  repository.updateLastReadAtResult = false;
  ContactService service(&network, &repository);

  service.updateLastReadAt("user-1");

  QCOMPARE(repository.updateLastReadAtCalls, 1);
  QVERIFY(network.sentPayloads.isEmpty());
}

void ContactServiceTest::contactsLoadedPersistsAndEmitsUsers() {
  FakeNetwork network;
  MockUserRepository repository;
  repository.storedUsers = {contactUser("user-1"), contactUser("user-2", "Online")};
  ContactService service(&network, &repository);
  QSignalSpy loadedSpy(&service, &IContactService::contactsLoaded);

  network.jsonReceived({{"type", "contacts_loaded"},
                        {"data", QJsonArray{contactUser("user-1").toJson(), contactUser("user-2").toJson()}}});

  QCOMPARE(repository.saveCalls, 2);
  QCOMPARE(repository.findAllCalls, 1);
  QCOMPARE(loadedSpy.count(), 1);
  QCOMPARE(loadedSpy.at(0).at(0).value<QList<User>>().size(), 2);
}

void ContactServiceTest::searchResponseEmitsUsers() {
  FakeNetwork network;
  MockUserRepository repository;
  ContactService service(&network, &repository);
  QSignalSpy searchSpy(&service, &IContactService::usersSearchLoaded);

  network.jsonReceived({{"type", "search_users_response"},
                        {"data", QJsonArray{contactUser("user-1").toJson()}}});

  QCOMPARE(searchSpy.count(), 1);
  QCOMPARE(searchSpy.at(0).at(0).value<QList<User>>().first().uuid(), QStringLiteral("user-1"));
  QCOMPARE(repository.saveCalls, 0);
}

void ContactServiceTest::resolveResponseSavesAndEmitsUser() {
  FakeNetwork network;
  MockUserRepository repository;
  ContactService service(&network, &repository);
  QSignalSpy resolvedSpy(&service, &IContactService::userResolved);

  network.jsonReceived({{"type", "resolve_user_response"}, {"data", contactUser().toJson()}});

  QCOMPARE(repository.saveCalls, 1);
  QCOMPARE(resolvedSpy.count(), 1);
  QCOMPARE(resolvedSpy.at(0).at(0).value<User>().uuid(), QStringLiteral("user-1"));
  QCOMPARE(network.sentPayloads.size(), 1);
  QCOMPARE(network.sentPayloads.first().value("type").toString(), QStringLiteral("add_contact"));
}

void ContactServiceTest::statusUpdateEmitsStatus() {
  FakeNetwork network;
  MockUserRepository repository;
  ContactService service(&network, &repository);
  QSignalSpy statusSpy(&service, &IContactService::contactStatusUpdated);

  network.jsonReceived({{"type", "contact_status_update"},
                        {"data", QJsonObject{{"uuid", "user-1"}, {"status", "Online"}}}});

  QCOMPARE(statusSpy.count(), 1);
  QCOMPARE(statusSpy.at(0).at(0).toString(), QStringLiteral("user-1"));
  QCOMPARE(statusSpy.at(0).at(1).toString(), QStringLiteral("Online"));
}

void ContactServiceTest::malformedResponsesEmitErrors() {
  FakeNetwork network;
  MockUserRepository repository;
  ContactService service(&network, &repository);
  QSignalSpy errorSpy(&service, &IContactService::contactError);

  network.jsonReceived({{"type", "contacts_loaded"}, {"data", QJsonObject{}}});
  network.jsonReceived({{"type", "resolve_user_response"}, {"data", QJsonObject{}}});
  network.jsonReceived({{"type", "resolve_user_response"}, {"data", contactUser().toJson()}});
  repository.saveResult = false;
  network.jsonReceived({{"type", "resolve_user_response"}, {"data", contactUser("user-2").toJson()}});

  QCOMPARE(errorSpy.count(), 3);
  QCOMPARE(errorSpy.at(0).at(0).toString(), QStringLiteral("Missing data field in server response"));
  QCOMPARE(errorSpy.at(1).at(0).toString(), QStringLiteral("Utilisateur résolu invalide"));
  QCOMPARE(errorSpy.at(2).at(0).toString(), QStringLiteral("Impossible de sauvegarder le contact"));
}

void ContactServiceTest::deleteAllAndDisconnectDelegate() {
  FakeNetwork network;
  MockUserRepository repository;
  ContactService service(&network, &repository);

  service.deleteAll();
  service.disconnectFromServer();

  QCOMPARE(repository.removeAllCalls, 1);
  QVERIFY(network.disconnected);
}