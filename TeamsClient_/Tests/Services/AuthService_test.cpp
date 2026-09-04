#include "AuthService_test.h"
#include "Services/Auth/AuthService.h"
#include "TestDoubles.h"
#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

namespace { User authUser() { return User("alice@example.com", "Alice", "Martin", "Online", false, "server-token", "user-1"); } }

void AuthServiceTest::loginAndRegisterSendPayloads() {
  FakeNetwork network; FakeLocalUserService local; FakeTokenManager token;
  AuthService service(&network, &local, nullptr, &token);
  service.loginUser("alice@example.com", "password");
  const QJsonObject loginPayload = network.sentPayloads.last();
  QCOMPARE(loginPayload.value("type").toString(), QStringLiteral("login"));
  QCOMPARE(loginPayload.value("email").toString(), QStringLiteral("alice@example.com"));
  QCOMPARE(loginPayload.value("password").toString(), QStringLiteral("password"));
  service.registerUser("Alice", "Martin", "alice@example.com", "password");
  const QJsonObject registerPayload = network.sentPayloads.last();
  QCOMPARE(registerPayload.value("type").toString(), QStringLiteral("register"));
  QCOMPARE(registerPayload.value("firstName").toString(), QStringLiteral("Alice"));
  QCOMPARE(registerPayload.value("lastName").toString(), QStringLiteral("Martin"));
  QCOMPARE(registerPayload.value("email").toString(), QStringLiteral("alice@example.com"));
  QCOMPARE(registerPayload.value("password").toString(), QStringLiteral("password"));
}

void AuthServiceTest::validResponseSavesUserAndEmitsSuccess() {
  FakeNetwork network; FakeLocalUserService local; FakeTokenManager token;
  AuthService service(&network, &local, nullptr, &token);
  QSignalSpy successSpy(&service, &IAuthService::authSuccess);
  network.jsonReceived({{"type", "login_response"}, {"data", authUser().toJson()}});
  QCOMPARE(successSpy.count(), 1);
  QCOMPARE(local.savedUser.uuid(), QStringLiteral("user-1"));
  QCOMPARE(local.savedUser.isMe(), true);
  QCOMPARE(token.token, QStringLiteral("server-token"));
  QCOMPARE(successSpy.count(), 1);
}

void AuthServiceTest::invalidResponseEmitsError() {
  FakeNetwork network; FakeLocalUserService local; FakeTokenManager token;
  AuthService service(&network, &local, nullptr, &token);
  QSignalSpy errorSpy(&service, &IAuthService::authError);
  network.jsonReceived({{"type", "login_response"}});
  QCOMPARE(errorSpy.count(), 1);
  QCOMPARE(errorSpy.at(0).at(0).toString(), QStringLiteral("Missing data field in server response"));
}

void AuthServiceTest::disconnectsNetwork() {
  FakeNetwork network; FakeLocalUserService local; FakeTokenManager token;
  AuthService service(&network, &local, nullptr, &token);
  service.disconnectFromServer();
  QVERIFY(network.disconnected);
}

void AuthServiceTest::loginWithTokenUsesTokenFlow() {
  FakeNetwork network; FakeLocalUserService local; FakeMessageService messages; FakeTokenManager token;
  AuthService service(&network, &local, &messages, &token);

  service.loginWithToken();

  QCOMPARE(network.sentPayloads.size(), 1);
  QCOMPARE(network.sentPayloads.first().value("type").toString(), QStringLiteral("validate_token"));
  QCOMPARE(network.sentPayloads.first().value("token").toString(), QStringLiteral("00f3f79fdc6f30cf40a50b078102caef"));
}

void AuthServiceTest::tokenErrorCleansLocalServices() {
  FakeNetwork network; FakeLocalUserService local; FakeMessageService messages; FakeTokenManager token;
  token.token = "token-1";
  AuthService service(&network, &local, &messages, &token);
  QSignalSpy noTokenSpy(&service, &IAuthService::noTokenFound);

  emit service.errorToken("Token is not valid");

  QVERIFY(token.deleted);
  QVERIFY(local.deleted);
  QVERIFY(messages.deleteCalled);
  QCOMPARE(noTokenSpy.count(), 1);
}

void AuthServiceTest::serverErrorsAreForwarded() {
  FakeNetwork network; FakeLocalUserService local; FakeTokenManager token;
  AuthService service(&network, &local, nullptr, &token);
  QSignalSpy errorSpy(&service, &IAuthService::authError);

  emit network.networkError("connection failed");

  QCOMPARE(errorSpy.count(), 1);
  QCOMPARE(errorSpy.at(0).at(0).toString(), QStringLiteral("connection failed"));
}