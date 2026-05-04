#include "RemoteVideo.h"

#include <QtMultimedia/qvideosink.h>

#include "../../../Utils/FrameConverter.h"

RemoteVideo::RemoteVideo(QWidget* parent) : QVideoWidget(parent) {
  setFixedSize(960, 540);

  Sources::instance().remoteVideo()->onFrame = [this](const webrtc::VideoFrame& frame) {
    webrtc::scoped_refptr<webrtc::I420BufferInterface> i420 = frame.video_frame_buffer()->ToI420();

    webrtc::scoped_refptr<webrtc::I420Buffer> scaled;
    if (i420->width() != width() || i420->height() != height()) {
      scaled = webrtc::I420Buffer::Create(width(), height());
      scaled->CropAndScaleFrom(*i420);
    } else {
      scaled = webrtc::I420Buffer::Create(i420->width(), i420->height());
      scaled->CropAndScaleFrom(*i420);
    }

    QVideoFrame qframe = FrameConverter::I420ToVideoFrame(scaled);
    if (!qframe.isValid()) return;

    QMetaObject::invokeMethod(
        this, [this, qframe]() { videoSink()->setVideoFrame(qframe); }, Qt::QueuedConnection);
  };
}