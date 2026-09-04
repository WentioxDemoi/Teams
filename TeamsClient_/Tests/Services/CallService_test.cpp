#include "CallService_test.h"
#include "Chat/Call/CallService.h"
#include "TestDoubles.h"
#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

void CallServiceTest::startCallSendsRequestAndOpensWindow() {
  FakeNetwork network; FakeWebRTCService webRTC; CallService service(&network, &webRTC);
  QSignalSpy openSpy(&service, &ICallService::openCallWindow);
  service.startCall("user-2", "Bob");
  QCOMPARE(openSpy.count(), 1);
  QCOMPARE(openSpy.at(0).at(0).toString(), QStringLiteral("Bob"));
  QCOMPARE(network.sentPayloads.first().value("type").toString(), QStringLiteral("call_request"));
  QCOMPARE(network.sentPayloads.first().value("targetUuid").toString(), QStringLiteral("user-2"));
}

void CallServiceTest::secondCallIsRejected() {
  FakeNetwork network; FakeWebRTCService webRTC; CallService service(&network, &webRTC);
  QSignalSpy errorSpy(&service, &ICallService::callError);
  service.startCall("user-2", "Bob");
  service.startCall("user-3", "Carol");
  QCOMPARE(errorSpy.count(), 1);
  QCOMPARE(errorSpy.at(0).at(0).toString(), QStringLiteral("Un appel est déjà en cours"));
  QCOMPARE(network.sentPayloads.size(), 1);
}

void CallServiceTest::signalingResponsesAreForwarded() {
  FakeNetwork network; FakeWebRTCService webRTC; CallService service(&network, &webRTC);
  QSignalSpy answerSpy(&service, &ICallService::answerReceived);
  QSignalSpy iceSpy(&service, &ICallService::iceReceived);
  network.jsonReceived({{"type", "answer"}, {"sdp", "answer-sdp"}});
  network.jsonReceived({{"type", "ice"}, {"candidate", "candidate"}, {"mid", "audio"}, {"index", 2}});
  QCOMPARE(answerSpy.count(), 1);
  QCOMPARE(answerSpy.at(0).at(0).toString(), QStringLiteral("answer-sdp"));
  QCOMPARE(iceSpy.count(), 1);
  QCOMPARE(iceSpy.at(0).at(2).toInt(), 2);
}

void CallServiceTest::incomingOfferCanBeAccepted() {
  FakeNetwork network; FakeWebRTCService webRTC; CallService service(&network, &webRTC);
  QSignalSpy incomingSpy(&service, &ICallService::incomingCallReceived);
  QSignalSpy openSpy(&service, &ICallService::openCallWindow);
  network.jsonReceived({{"type", "offer"}, {"senderUuid", "user-2"}, {"sdp", "offer-sdp"}});
  service.acceptCall("Bob");
  QCOMPARE(incomingSpy.count(), 1);
  QCOMPARE(openSpy.count(), 1);
  QCOMPARE(webRTC.remoteOffer, QStringLiteral("offer-sdp"));
  QCOMPARE(network.sentPayloads.last().value("type").toString(), QStringLiteral("call_accept"));
}

void CallServiceTest::hangupSendsRequestAndClosesWindow() {
  FakeNetwork network; FakeWebRTCService webRTC; CallService service(&network, &webRTC);
  QSignalSpy closeSpy(&service, &ICallService::closeCallWindow);
  service.startCall("user-2", "Bob");
  service.hangup();
  QVERIFY(webRTC.hangupCalled);
  QCOMPARE(closeSpy.count(), 1);
  QCOMPARE(network.sentPayloads.last().value("type").toString(), QStringLiteral("call_hangup"));
}

void CallServiceTest::rejectCallSendsRequest() {
  FakeNetwork network; FakeWebRTCService webRTC; CallService service(&network, &webRTC);

  service.rejectCall();

  QCOMPARE(network.sentPayloads.size(), 1);
  QCOMPARE(network.sentPayloads.first().value("type").toString(), QStringLiteral("call_reject"));
  QCOMPARE(network.sentPayloads.first().value("targetUuid").toString(), QString());
}

void CallServiceTest::cameraChangeSendsRequest() {
  FakeNetwork network; FakeWebRTCService webRTC; CallService service(&network, &webRTC);

  service.cameraEnabledChanged(true);

  QCOMPARE(network.sentPayloads.size(), 1);
  const QJsonObject payload = network.sentPayloads.first();
  QCOMPARE(payload.value("type").toString(), QStringLiteral("camera_enabled_change"));
  QCOMPARE(payload.value("value").toString(), QStringLiteral("true"));
  QCOMPARE(payload.value("targetUuid").toString(), QString());
}

void CallServiceTest::disconnectsNetwork() {
  FakeNetwork network; FakeWebRTCService webRTC; CallService service(&network, &webRTC);
  service.disconnectFromServer();
  QVERIFY(network.disconnected);
}

void CallServiceTest::invalidIncomingOfferIsIgnored() {
  FakeNetwork network; FakeWebRTCService webRTC; CallService service(&network, &webRTC);
  QSignalSpy incomingSpy(&service, &ICallService::incomingCallReceived);

  network.jsonReceived({{"type", "offer"}, {"senderUuid", "user-2"}, {"sdp", 42}});

  QCOMPARE(incomingSpy.count(), 0);
  QVERIFY(network.sentPayloads.isEmpty());
}

void CallServiceTest::requestAckStartsWebRtcOffer() {
  FakeNetwork network;
  FakeWebRTCService webRTC;
  CallService service(&network, &webRTC);

  service.startCall("user-2", "Bob");
  network.jsonReceived({{"type", "call_request_ack"}});

  QVERIFY(webRTC.startCalled);
}

void CallServiceTest::localWebRtcCallbacksSendSignalingMessages() {
  FakeNetwork network;
  FakeWebRTCService webRTC;
  CallService service(&network, &webRTC);
  service.startCall("user-2", "Bob");

  webRTC.onOffer("offer-sdp");
  QTRY_COMPARE(network.sentPayloads.size(), 2);
  QCOMPARE(network.sentPayloads.last().value("type").toString(), QStringLiteral("offer"));
  QCOMPARE(network.sentPayloads.last().value("targetUuid").toString(), QStringLiteral("user-2"));
  QCOMPARE(network.sentPayloads.last().value("sdp").toString(), QStringLiteral("offer-sdp"));

  webRTC.onAnswer("answer-sdp");
  QTRY_COMPARE(network.sentPayloads.size(), 3);
  QCOMPARE(network.sentPayloads.last().value("type").toString(), QStringLiteral("answer"));
  QCOMPARE(network.sentPayloads.last().value("sdp").toString(), QStringLiteral("answer-sdp"));

  webRTC.onIce("candidate", "audio", 4);
  QTRY_COMPARE(network.sentPayloads.size(), 4);
  QCOMPARE(network.sentPayloads.last().value("type").toString(), QStringLiteral("ice"));
  QCOMPARE(network.sentPayloads.last().value("candidate").toString(), QStringLiteral("candidate"));
  QCOMPARE(network.sentPayloads.last().value("mid").toString(), QStringLiteral("audio"));
  QCOMPARE(network.sentPayloads.last().value("index").toInt(), 4);
}

void CallServiceTest::remoteAnswerAndIceReachWebRtc() {
  FakeNetwork network;
  FakeWebRTCService webRTC;
  CallService service(&network, &webRTC);

  network.jsonReceived({{"type", "answer"}, {"sdp", "answer-sdp"}});
  network.jsonReceived({{"type", "ice"}, {"candidate", "candidate"}, {"mid", "video"}, {"index", 3}});

  QCOMPARE(webRTC.remoteAnswer, QStringLiteral("answer-sdp"));
  QCOMPARE(webRTC.remoteCandidate, QStringLiteral("candidate"));
  QCOMPARE(webRTC.remoteMid, QStringLiteral("video"));
  QCOMPARE(webRTC.remoteIndex, 3);
}

void CallServiceTest::remoteCallEventsCloseOrCancelCall() {
  const QList<QString> closingEvents{"callee_offline", "call_reject", "call_hangup", "busy"};
  for (const QString& event : closingEvents) {
    FakeNetwork network;
    FakeWebRTCService webRTC;
    CallService service(&network, &webRTC);
    QSignalSpy closeSpy(&service, &ICallService::closeCallWindow);
    service.startCall("user-2", "Bob");

    network.jsonReceived({{"type", event}});

    QCOMPARE(closeSpy.count(), 1);
    QVERIFY(webRTC.hangupCalled);
  }

  FakeNetwork network;
  FakeWebRTCService webRTC;
  CallService service(&network, &webRTC);
  QSignalSpy cancelledSpy(&service, &ICallService::incomingCallCancelled);
  network.jsonReceived({{"type", "call_cancel"}, {"senderUuid", "user-2"}});
  QCOMPARE(cancelledSpy.count(), 1);
  QCOMPARE(cancelledSpy.at(0).at(0).toString(), QStringLiteral("user-2"));
  QVERIFY(webRTC.hangupCalled);
}

void CallServiceTest::acceptWithoutOfferEmitsError() {
  FakeNetwork network;
  FakeWebRTCService webRTC;
  CallService service(&network, &webRTC);
  QSignalSpy errorSpy(&service, &ICallService::callError);

  service.acceptCall("Bob");

  QCOMPARE(errorSpy.count(), 1);
  QCOMPARE(errorSpy.at(0).at(0).toString(), QStringLiteral("Aucun appel entrant à accepter"));
  QVERIFY(network.sentPayloads.isEmpty());
}

void CallServiceTest::offerDuringCallSendsBusyResponse() {
  FakeNetwork network;
  FakeWebRTCService webRTC;
  CallService service(&network, &webRTC);
  service.startCall("user-1", "Alice");

  network.jsonReceived({{"type", "offer"}, {"senderUuid", "user-2"}, {"sdp", "offer-sdp"}});

  QCOMPARE(network.sentPayloads.size(), 2);
  QCOMPARE(network.sentPayloads.last().value("type").toString(), QStringLiteral("busy"));
  QCOMPARE(network.sentPayloads.last().value("targetUuid").toString(), QStringLiteral("user-2"));
}

void CallServiceTest::cameraCanBeDisabled() {
  FakeNetwork network;
  FakeWebRTCService webRTC;
  CallService service(&network, &webRTC);

  service.cameraEnabledChanged(false);

  QCOMPARE(network.sentPayloads.size(), 1);
  QCOMPARE(network.sentPayloads.first().value("value").toString(), QStringLiteral("false"));
}

void CallServiceTest::remoteStateSignalsAreForwarded() {
  FakeNetwork network;
  FakeWebRTCService webRTC;
  CallService service(&network, &webRTC);
  QSignalSpy connectedSpy(&service, &ICallService::isContactConnectedChanged);
  QSignalSpy cameraSpy(&service, &ICallService::onCameraEnabledChanged);

  network.jsonReceived({{"type", "call_accept"}});
  network.jsonReceived({{"type", "camera_enabled_change"}, {"value", "false"}});

  QCOMPARE(connectedSpy.count(), 1);
  QCOMPARE(connectedSpy.at(0).at(0).toBool(), true);
  QCOMPARE(cameraSpy.count(), 1);
  QCOMPARE(cameraSpy.at(0).at(0).toBool(), false);
}

void CallServiceTest::networkErrorsAreForwarded() {
  FakeNetwork network;
  FakeWebRTCService webRTC;
  CallService service(&network, &webRTC);
  QSignalSpy errorSpy(&service, &ICallService::callError);

  emit network.networkError("network failure");

  QCOMPARE(errorSpy.count(), 1);
  QCOMPARE(errorSpy.at(0).at(0).toString(), QStringLiteral("network failure"));
}

void CallServiceTest::invalidServerMessagesAreIgnored() {
  FakeNetwork network;
  FakeWebRTCService webRTC;
  CallService service(&network, &webRTC);
  QSignalSpy answerSpy(&service, &ICallService::answerReceived);
  QSignalSpy iceSpy(&service, &ICallService::iceReceived);

  network.jsonReceived(QJsonObject{});
  network.jsonReceived({{"type", 42}});
  network.jsonReceived({{"type", "camera_enabled_change"}, {"value", 42}});

  QCOMPARE(answerSpy.count(), 0);
  QCOMPARE(iceSpy.count(), 0);
}

void CallServiceTest::timeoutCancelsCall() {
  FakeNetwork network;
  FakeWebRTCService webRTC;
  CallService service(&network, &webRTC);
  QSignalSpy closeSpy(&service, &ICallService::closeCallWindow);
  service.startCall("user-2", "Bob");

  QTimer* timer = service.findChild<QTimer*>();
  QVERIFY(timer != nullptr);
  QVERIFY(QMetaObject::invokeMethod(timer, "timeout", Qt::DirectConnection));

  QCOMPARE(closeSpy.count(), 1);
  QCOMPARE(network.sentPayloads.last().value("type").toString(), QStringLiteral("call_cancel"));
  QCOMPARE(network.sentPayloads.last().value("targetUuid").toString(), QStringLiteral("user-2"));
}

void CallServiceTest::hangupOutsideCallOnlyStopsWebRtc() {
  FakeNetwork network;
  FakeWebRTCService webRTC;
  CallService service(&network, &webRTC);

  service.hangup();

  QVERIFY(webRTC.hangupCalled);
  QVERIFY(network.sentPayloads.isEmpty());
}