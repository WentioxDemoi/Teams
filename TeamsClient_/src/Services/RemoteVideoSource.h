#ifndef REMOTEVIDEOSOURCE_H
#define REMOTEVIDEOSOURCE_H
#include "../includes.h"
#include "../webrtc_includes.h"

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