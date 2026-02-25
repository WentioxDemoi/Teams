#include "LocalVideo.h"
#include "Sources.h"

LocalVideo::LocalVideo(QWidget *parent) : QWidget(parent) {
  setFixedSize(320, 180);
  sink = new QVideoSink(this);
  videoWidget = new QVideoWidget(this);
  videoWidget->setGeometry(rect());

  camera = new QCamera(QMediaDevices::defaultVideoInput(), this);
  captureSession = new QMediaCaptureSession(this);

  captureSession->setCamera(camera);
  captureSession->setVideoOutput(sink);

  connect(sink, &QVideoSink::videoFrameChanged, this,
          &LocalVideo::OnFrameChanged);
  camera->start();
  videoSource = Sources::instance().localVideo();
}

void LocalVideo::OnFrameChanged(const QVideoFrame &frame) {
  if (!frame.isValid())
    return;
  videoWidget->videoSink()->setVideoFrame(frame);

  if (p2pInProgress) {
    webrtc::scoped_refptr<webrtc::I420BufferInterface> i420Buffer;
    // Ajouter des formats a terme
    switch (frame.surfaceFormat().pixelFormat()) {
    case QVideoFrameFormat::PixelFormat::Format_NV12:
      i420Buffer = FrameConverter::NV12ToI420(frame);
      break;
    default:
      qWarning() << "Format non supporté pour WebRTC:"
                 << frame.surfaceFormat().pixelFormat();
      return;
    }

    videoSource->PushFrame(i420Buffer);
    // SendFrameToWebRTC(i420Buffer);
  }
}

void LocalVideo::OnP2PChange(bool InProgress) { p2pInProgress = InProgress; }