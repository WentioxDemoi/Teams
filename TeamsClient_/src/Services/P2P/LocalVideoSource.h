// LocalVideoSource.h
#ifndef LOCALVIDEOSOURCE_H
#define LOCALVIDEOSOURCE_H

#include "../../webrtc_includes.h"
#include "media/base/adapted_video_track_source.h"
#include "rtc_base/time_utils.h"
#include "absl/types/optional.h"


class LocalVideoSource : public webrtc::AdaptedVideoTrackSource {
public:
    LocalVideoSource() {};
    void PushFrame(const webrtc::scoped_refptr<webrtc::I420BufferInterface>& buffer) {
        int64_t timestampUs = webrtc::TimeMicros();
        webrtc::VideoFrame frame = webrtc::VideoFrame::Builder()
            .set_video_frame_buffer(buffer)
            .set_timestamp_us(timestampUs)
            .build();
        OnFrame(frame); // injecte dans WebRTC
    }

    // Implémentations obligatoires
    bool is_screencast() const override { return false; }
    absl::optional<bool> needs_denoising() const override { return false; }
    webrtc::MediaSourceInterface::SourceState state() const override {
        return webrtc::MediaSourceInterface::kLive;
    }
    bool remote() const override { return false; }

    WEBRTC_REF_COUNT_IMPL;
};

#endif