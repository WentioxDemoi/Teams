#ifndef REMOTEVIDEOSOURCE_H
#define REMOTEVIDEOSOURCE_H

#include <QtMultimedia/qvideoframe.h>
#include <functional>
#include <rtc_base/synchronization/mutex.h>

#include "../../webrtc_includes.h"
#include "api/ref_count.h"
#include "api/video/video_frame.h"
#include "api/video/video_sink_interface.h"
#include "FrameConverter.h"

/**
 * @class RemoteVideoSource
 * @brief Source vidéo distante.
 *
 * Reçoit les frames vidéo d'un pair distant et les transmet via un callback.
 * Implémente les interfaces WebRTC pour le comptage de références et la réception
 * des frames vidéo.
 */
class RemoteVideoSource : public webrtc::RefCountInterface,
                          public webrtc::VideoSinkInterface<webrtc::VideoFrame> {
 public:
  RemoteVideoSource() = default;

    void setFrameCallback(std::function<void(const QVideoFrame&)> cb) {
        webrtc::MutexLock lock(&mutex_);
        onFrame = std::move(cb);
    }

  void OnFrame(const webrtc::VideoFrame& frame) override {
    auto i420 = frame.video_frame_buffer()->ToI420();
    QVideoFrame qFrame = FrameConverter::I420ToVideoFrame(i420);
    webrtc::MutexLock lock(&mutex_);
    if (onFrame) onFrame(qFrame);
  }
public:
  WEBRTC_REF_COUNT_IMPL;

private:
  std::function<void(const QVideoFrame&)> onFrame;
  webrtc::Mutex mutex_;
};
#endif