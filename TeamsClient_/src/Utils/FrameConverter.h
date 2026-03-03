#ifndef FRAMECONVERTER_H
#define FRAMECONVERTER_H

#include "../webrtc_includes.h"
#include "api/video/i420_buffer.h"
#include "scoped_refptr.h"
#include <QVideoFrame>

class FrameConverter {
 public:
  static webrtc::scoped_refptr<webrtc::I420Buffer> NV12ToI420(const QVideoFrame& nv12Frame);
  static QVideoFrame I420ToVideoFrame(webrtc::scoped_refptr<webrtc::I420Buffer> i420Buffer);
};

#endif