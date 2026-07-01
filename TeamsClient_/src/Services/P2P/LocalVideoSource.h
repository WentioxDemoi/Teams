#ifndef LOCALVIDEOSOURCE_H
#define LOCALVIDEOSOURCE_H

#include "../../webrtc_includes.h"
#include "absl/types/optional.h"
#include "media/base/adapted_video_track_source.h"
#include "rtc_base/time_utils.h"
#include <QtMultimedia/qvideoframe.h>
#include "FrameConverter.h"

/**
 * @class LocalVideoSource
 * @brief Source vidéo locale pour WebRTC.
 *
 * Capture et fournit les frames vidéo locales à WebRTC. Implémente
 * les méthodes nécessaires pour indiquer que la source est active,
 * non distante et ne nécessite pas de débruitage, et injecte les frames
 * dans le pipeline WebRTC.
 */
class LocalVideoSource : public webrtc::AdaptedVideoTrackSource {
 public:
 using FrameCallback = std::function<void(QVideoFrame)>;
 

//  void setPreviewCallback(FrameCallback cb) {
//         previewCallback_ = std::move(cb);
//     }

  LocalVideoSource() {};
  void PushFrame(const webrtc::scoped_refptr<webrtc::I420BufferInterface>& buffer) {
    int64_t timestampUs = webrtc::TimeMicros();
    webrtc::VideoFrame frame = webrtc::VideoFrame::Builder()
                                   .set_video_frame_buffer(buffer)
                                   .set_timestamp_us(timestampUs)
                                   .build();
    OnFrame(frame);  // injecte dans WebRTC

    // if (previewCallback_) {
    //     previewCallback_(FrameConverter::I420ToVideoFrame(buffer));
    // }
  }

  // Implémentations obligatoires
  bool is_screencast() const override { return false; }
  absl::optional<bool> needs_denoising() const override { return false; }
  webrtc::MediaSourceInterface::SourceState state() const override {
    return webrtc::MediaSourceInterface::kLive;
  }
  bool remote() const override { return false; }

  WEBRTC_REF_COUNT_IMPL;

  private:
  // FrameCallback previewCallback_;
};

#endif