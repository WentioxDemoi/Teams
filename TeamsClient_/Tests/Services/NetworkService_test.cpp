#include "NetworkService_test.h"

#include "Services/Network/NetworkService.h"
#include "Core/State/UserState.h"

#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

void NetworkServiceTest::emptyPayloadIsIgnored() {
  NetworkService network(0);
  QSignalSpy receivedSpy(&network, &INetworkService::jsonReceived);

  network.send(QJsonObject{});

  QCOMPARE(receivedSpy.count(), 0);
  QCOMPARE(network.pendingMessageCount(), 0);
}

void NetworkServiceTest::validJsonIsEmitted() {
  NetworkService network(0);
  QSignalSpy receivedSpy(&network, &INetworkService::jsonReceived);

  network.handleIncomingData(R"({"type":"login_response","ok":true})");

  QCOMPARE(receivedSpy.count(), 1);
  QCOMPARE(receivedSpy.at(0).at(0).toJsonObject().value("type").toString(), QStringLiteral("login_response"));
  QCOMPARE(receivedSpy.at(0).at(0).toJsonObject().value("ok").toBool(), true);
}

void NetworkServiceTest::malformedJsonEmitsError() {
  NetworkService network(0);
  QSignalSpy errorSpy(&network, &INetworkService::networkError);

  network.handleIncomingData("not-json");

  QCOMPARE(errorSpy.count(), 1);
  QCOMPARE(errorSpy.at(0).at(0).toString(), QStringLiteral("Malformed JSON received from server"));
}

void NetworkServiceTest::missingTypeEmitsError() {
  NetworkService network(0);
  QSignalSpy errorSpy(&network, &INetworkService::networkError);

  network.handleIncomingData(R"({"data":{}})");

  QCOMPARE(errorSpy.count(), 1);
  QCOMPARE(errorSpy.at(0).at(0).toString(), QStringLiteral("Missing or invalid type field in server response"));
}

void NetworkServiceTest::newlineDelimitedMessagesAreParsed() {
  NetworkService network(0);
  QSignalSpy receivedSpy(&network, &INetworkService::jsonReceived);

  network.handleIncomingData(R"({"type":"first"})");
  network.handleIncomingData(R"({"type":"second"})");

  QCOMPARE(receivedSpy.count(), 2);
  QCOMPARE(receivedSpy.at(1).at(0).toJsonObject().value("type").toString(), QStringLiteral("second"));
}

void NetworkServiceTest::emptyIncomingDataIsIgnored() {
  NetworkService network(0);
  QSignalSpy receivedSpy(&network, &INetworkService::jsonReceived);
  QSignalSpy errorSpy(&network, &INetworkService::networkError);

  network.handleIncomingData(QByteArray{});

  QCOMPARE(receivedSpy.count(), 0);
  QCOMPARE(errorSpy.count(), 0);
}

void NetworkServiceTest::nonObjectJsonEmitsError() {
  NetworkService network(0);
  QSignalSpy errorSpy(&network, &INetworkService::networkError);

  network.handleIncomingData("[1,2,3]");

  QCOMPARE(errorSpy.count(), 1);
  QCOMPARE(errorSpy.at(0).at(0).toString(), QStringLiteral("Malformed JSON received from server"));
}

void NetworkServiceTest::invalidTypeEmitsError() {
  NetworkService network(0);
  QSignalSpy errorSpy(&network, &INetworkService::networkError);

  network.handleIncomingData(R"({"type":42})");

  QCOMPARE(errorSpy.count(), 1);
  QCOMPARE(errorSpy.at(0).at(0).toString(), QStringLiteral("Missing or invalid type field in server response"));
}

void NetworkServiceTest::nonEmptyPayloadIsQueuedUntilEncrypted() {
  NetworkService network(0);

  network.send({{"type", "ping"}});

  QCOMPARE(network.pendingMessageCount(), 1);
}

void NetworkServiceTest::savingUserQueuesAuthenticationPayload() {
  UserState::instance().deleteLocalUser();
  NetworkService network(0);

  UserState::instance().saveLocalUser(
      User("network@example.com", "Network", "User", "Online", true, "token-1", "network-test-user"));

  QCOMPARE(network.pendingMessageCount(), 1);
  UserState::instance().deleteLocalUser();
}

void NetworkServiceTest::disconnectPreservesPendingQueue() {
  NetworkService network(0);
  network.send({{"type", "queued"}});
  QCOMPARE(network.pendingMessageCount(), 1);

  network.disconnectFromServer();

  QCOMPARE(network.pendingMessageCount(), 1);
}