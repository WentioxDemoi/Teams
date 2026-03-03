#ifndef LOCALVIDEO_H
#define LOCALVIDEO_H

#include <api/scoped_refptr.h>
#include <api/video/i420_buffer.h>
#include <api/video/video_frame.h>

#include <QCamera>
#include <QDebug>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QVideoFrame>
#include <QVideoFrameFormat>
#include <QVideoSink>
#include <QVideoWidget>
#include <QWidget>

#include "../../../Services/P2P/LocalVideoSource.h"

/**
 * @class LocalVideo
 * @brief Widget de capture et d'affichage de la vidéo locale.
 *
 * Gère la capture vidéo depuis la caméra locale et l'affichage dans l'interface.
 */
class LocalVideo : public QWidget {
  Q_OBJECT

 public:
  explicit LocalVideo(QWidget* parent = nullptr);

 private slots:
  void OnFrameChanged(const QVideoFrame& frame);

 public slots:
  void OnP2PChange(bool inProgress);

 protected:
 private:
  QVideoSink* sink = nullptr;
  QCamera* camera = nullptr;
  QMediaCaptureSession* captureSession = nullptr;
  QVideoWidget* videoWidget = nullptr;
  bool p2pInProgress = false;
  webrtc::scoped_refptr<LocalVideoSource> videoSource;
};

#endif