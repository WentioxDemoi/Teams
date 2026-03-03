#ifndef REMOTEVIDEOSOURCE_H
#define REMOTEVIDEOSOURCE_H

#include "../../webrtc_includes.h"
#include "api/ref_count.h"
#include "api/video/video_sink_interface.h"
#include "api/video/video_frame.h"
#include <functional>

class RemoteVideoSource
    : public webrtc::RefCountInterface,
      public webrtc::VideoSinkInterface<webrtc::VideoFrame>
{
public:
    RemoteVideoSource() = default;

    void OnFrame(const webrtc::VideoFrame& frame) override {
        if (onFrame) onFrame(frame);
    }

    std::function<void(const webrtc::VideoFrame&)> onFrame;

    WEBRTC_REF_COUNT_IMPL;
};
#endif