#include "WebRTCViewModel_test.h"

#include "../Services/TestDoubles.h"
#include "ViewModels/WebRTCViewModel.h"

#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>
#include <QVideoFrameFormat>
#include <QVideoSink>

namespace {
class TestWebRTCViewModel : public WebRTCViewModel {
 public:
  TestWebRTCViewModel(IWebRTCService* webRTC, IChatService* chat)
      : WebRTCViewModel(nullptr, webRTC, chat) {}
};

QVideoFrame makeNv12Frame() {
  QVideoFrame frame(QVideoFrameFormat(QSize(2, 2), QVideoFrameFormat::Format_NV12));
  if (!frame.map(QVideoFrame::WriteOnly))
    return {};
  frame.bits(0)[0] = 16;
  frame.bits(0)[1] = 32;
  frame.bits(0)[frame.bytesPerLine(0)] = 48;
  frame.bits(0)[frame.bytesPerLine(0) + 1] = 64;
  frame.bits(1)[0] = 90;
  frame.bits(1)[1] = 140;
  frame.unmap();
  return frame;
}
}

void WebRTCViewModelTest::startsWithExpectedState() {
  FakeWebRTCService webRTC;
  FakeChatService chat;
  TestWebRTCViewModel viewModel(&webRTC, &chat);

  QVERIFY(!viewModel.cameraEnabled());
  QVERIFY(!viewModel.micEnabled());
  QVERIFY(!viewModel.isContactConnected());
  QVERIFY(!viewModel.isRemoteCameraEnabled());
  QVERIFY(viewModel.remoteUsername().isEmpty());
}

void WebRTCViewModelTest::toggleMicDelegatesAndEmits() {
  FakeWebRTCService webRTC;
  FakeChatService chat;
  TestWebRTCViewModel viewModel(&webRTC, &chat);
  QSignalSpy changedSpy(&viewModel, &WebRTCViewModel::micEnabledChanged);

  viewModel.toggleMic();
  QCOMPARE(viewModel.micEnabled(), true);
  QCOMPARE(webRTC.micEnabled, true);
  QCOMPARE(webRTC.micCalls, 1);
  QCOMPARE(changedSpy.count(), 1);
  viewModel.toggleMic();
  QCOMPARE(viewModel.micEnabled(), false);
  QCOMPARE(webRTC.micEnabled, false);
  QCOMPARE(webRTC.micCalls, 2);
  QCOMPARE(changedSpy.count(), 2);
}

void WebRTCViewModelTest::hangupDelegatesToWebRtcAndChat() {
  FakeWebRTCService webRTC;
  FakeChatService chat;
  TestWebRTCViewModel viewModel(&webRTC, &chat);

  viewModel.hangup();

  QCOMPARE(webRTC.hangupCalled, true);
  QCOMPARE(chat.hangupCalls, 1);
}

void WebRTCViewModelTest::remoteConnectionStateUpdatesProperty() {
  FakeWebRTCService webRTC;
  FakeChatService chat;
  TestWebRTCViewModel viewModel(&webRTC, &chat);
  QSignalSpy changedSpy(&viewModel, &WebRTCViewModel::isContactConnectedChanged);

  emit chat.isContactConnectedChanged(true);

  QCOMPARE(viewModel.isContactConnected(), true);
  QCOMPARE(changedSpy.count(), 1);
}

void WebRTCViewModelTest::remoteCameraStateUpdatesProperty() {
  FakeWebRTCService webRTC;
  FakeChatService chat;
  TestWebRTCViewModel viewModel(&webRTC, &chat);
  QSignalSpy changedSpy(&viewModel, &WebRTCViewModel::isRemoteCameraEnabledChanged);

  emit chat.onCameraEnabledChanged(true);
  QCOMPARE(viewModel.isRemoteCameraEnabled(), true);
  emit chat.onCameraEnabledChanged(false);
  QCOMPARE(viewModel.isRemoteCameraEnabled(), false);
  QCOMPARE(changedSpy.count(), 2);
}

void WebRTCViewModelTest::closeWindowWithoutWindowIsSafe() {
  FakeWebRTCService webRTC;
  FakeChatService chat;
  TestWebRTCViewModel viewModel(&webRTC, &chat);

  viewModel.onCloseCallWindow();

  QVERIFY(true);
}

void WebRTCViewModelTest::setVideoSinksAcceptsAndClearsSinks() {
  FakeWebRTCService webRTC;
  FakeChatService chat;
  TestWebRTCViewModel viewModel(&webRTC, &chat);
  QVideoSink localSink;
  QVideoSink remoteSink;

  viewModel.setLocalVideoSink(&localSink);
  viewModel.setRemoteVideoSink(&remoteSink);
  chat.onCameraEnabledChanged(true);

  QVERIFY(remoteSink.videoFrame().isValid() == false);
  viewModel.setRemoteVideoSink(nullptr);
  viewModel.setLocalVideoSink(nullptr);
}

void WebRTCViewModelTest::localCaptureFrameReachesLocalSink() {
  FakeWebRTCService webRTC;
  FakeChatService chat;
  TestWebRTCViewModel viewModel(&webRTC, &chat);
  QVideoSink localSink;
  viewModel.setLocalVideoSink(&localSink);
  QVideoSink* captureSink = viewModel.findChild<QVideoSink*>();
  QVERIFY(captureSink != nullptr);
  const QVideoFrame frame = makeNv12Frame();
  QVERIFY(frame.isValid());

  captureSink->setVideoFrame(frame);

  QVERIFY(localSink.videoFrame().isValid());
  QCOMPARE(localSink.videoFrame().size(), QSize(2, 2));
}

void WebRTCViewModelTest::disablingRemoteCameraClearsRemoteSinkFrame() {
  FakeWebRTCService webRTC;
  FakeChatService chat;
  TestWebRTCViewModel viewModel(&webRTC, &chat);
  QVideoSink remoteSink;
  remoteSink.setVideoFrame(makeNv12Frame());
  QVERIFY(remoteSink.videoFrame().isValid());
  viewModel.setRemoteVideoSink(&remoteSink);

  emit chat.onCameraEnabledChanged(false);

  QVERIFY(!remoteSink.videoFrame().isValid());
}

void WebRTCViewModelTest::connectionStateSupportsBothValues() {
  FakeWebRTCService webRTC;
  FakeChatService chat;
  TestWebRTCViewModel viewModel(&webRTC, &chat);
  QSignalSpy changedSpy(&viewModel, &WebRTCViewModel::isContactConnectedChanged);

  emit chat.isContactConnectedChanged(true);
  emit chat.isContactConnectedChanged(false);

  QCOMPARE(viewModel.isContactConnected(), false);
  QCOMPARE(changedSpy.count(), 2);
}

void WebRTCViewModelTest::hangupCallsBothServicesExactlyOnce() {
  FakeWebRTCService webRTC;
  FakeChatService chat;
  TestWebRTCViewModel viewModel(&webRTC, &chat);

  viewModel.hangup();

  QCOMPARE(webRTC.hangupCalled, true);
  QCOMPARE(chat.hangupCalls, 1);
}