#ifndef REMOTEVIDEOSOURCE_H
#define REMOTEVIDEOSOURCE_H

#include <functional>

#include "../../webrtc_includes.h"
#include "api/ref_count.h"
#include "api/video/video_frame.h"
#include "api/video/video_sink_interface.h"

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

  void OnFrame(const webrtc::VideoFrame& frame) override {
    if (onFrame) onFrame(frame);
  }

  std::function<void(const webrtc::VideoFrame&)> onFrame;

  WEBRTC_REF_COUNT_IMPL;
};
#endif