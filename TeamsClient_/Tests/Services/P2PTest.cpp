#include "P2PTest.h"

#include "Services/P2P/LocalVideoSource.h"
#include "Services/P2P/RemoteVideoSource.h"
#include "Services/P2P/Sources.h"

#include <QtTest/QtTest>

namespace {
webrtc::scoped_refptr<webrtc::I420Buffer> makeBuffer() {
  const auto buffer = webrtc::I420Buffer::Create(2, 2);
  for (int index = 0; index < 4; ++index) {
    buffer->MutableDataY()[index] = static_cast<uint8_t>(16 + index * 20);
  }
  buffer->MutableDataU()[0] = 90;
  buffer->MutableDataV()[0] = 140;
  return buffer;
}
}

void P2PTest::sourcesAreSingletonAndExposeBothVideoSources() {
  Sources& first = Sources::instance();
  Sources& second = Sources::instance();

  QCOMPARE(&first, &second);
  QVERIFY(first.localVideo() != nullptr);
  QVERIFY(first.remoteVideo() != nullptr);
  QCOMPARE(first.localVideo().get(), Sources::instance().localVideo().get());
  QCOMPARE(first.remoteVideo().get(), Sources::instance().remoteVideo().get());
}

void P2PTest::localVideoSourceReportsLiveLocalState() {
  const auto source = webrtc::scoped_refptr<LocalVideoSource>(new LocalVideoSource());

  QVERIFY(!source->is_screencast());
  QVERIFY(!source->remote());
  QVERIFY(source->needs_denoising().has_value());
  QVERIFY(!source->needs_denoising().value());
  QCOMPARE(source->state(), webrtc::MediaSourceInterface::kLive);
}

void P2PTest::localVideoSourcePushesI420Frames() {
  const auto source = webrtc::scoped_refptr<LocalVideoSource>(new LocalVideoSource());

  source->PushFrame(makeBuffer());

  QVERIFY(true);
}

void P2PTest::remoteVideoSourceForwardsConvertedFrames() {
  const auto source = webrtc::scoped_refptr<RemoteVideoSource>(new RemoteVideoSource());
  int callbackCount = 0;
  QVideoFrame receivedFrame;
  source->setFrameCallback([&](const QVideoFrame& frame) {
    ++callbackCount;
    receivedFrame = frame;
  });

  const webrtc::VideoFrame frame = webrtc::VideoFrame::Builder()
                                       .set_video_frame_buffer(makeBuffer())
                                       .set_timestamp_us(1234)
                                       .build();
  source->OnFrame(frame);

  QCOMPARE(callbackCount, 1);
  QVERIFY(receivedFrame.isValid());
  QCOMPARE(receivedFrame.size(), QSize(2, 2));
  QCOMPARE(receivedFrame.pixelFormat(), QVideoFrameFormat::Format_NV12);
}

void P2PTest::remoteVideoSourceWithoutCallbackIsSafe() {
  const auto source = webrtc::scoped_refptr<RemoteVideoSource>(new RemoteVideoSource());

  source->OnFrame(webrtc::VideoFrame::Builder()
                      .set_video_frame_buffer(makeBuffer())
                      .set_timestamp_us(1)
                      .build());

  QVERIFY(true);
}