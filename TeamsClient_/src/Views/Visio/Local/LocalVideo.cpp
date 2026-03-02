#include "LocalVideo.h"



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

    // ✅ Vérifier que la conversion a réussi
        if (!i420Buffer) {
            qWarning() << "Échec conversion I420 : buffer null";
            return;
        }

        // ✅ Vérifier les dimensions cohérentes
        if (i420Buffer->width() <= 0 || i420Buffer->height() <= 0) {
            qWarning() << "Buffer I420 invalide : dimensions nulles";
            return;
        }

        // ✅ Vérifier que les strides sont valides (largeur % 2 == 0 pour I420)
        if (i420Buffer->width() % 2 != 0 || i420Buffer->height() % 2 != 0) {
            qWarning() << "Dimensions non paires, I420 invalide";
            return;
        }

        // ✅ Vérifier que les plans UV ne sont pas null
        if (!i420Buffer->DataY() || !i420Buffer->DataU() || !i420Buffer->DataV()) {
            qWarning() << "Plans YUV null dans le buffer I420";
            return;
        }

    videoSource->PushFrame(i420Buffer);
    // SendFrameToWebRTC(i420Buffer);
  }
}

void LocalVideo::OnP2PChange(bool InProgress) { 
  qDebug() << "OnP2PChange";
  p2pInProgress = InProgress; }