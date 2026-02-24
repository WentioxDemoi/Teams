#ifndef FRAMECONVERTER_H
#define FRAMECONVERTER_H

#include "../includes.h"


class FrameConverter
{
public:
    static webrtc::scoped_refptr<webrtc::I420Buffer> NV12ToI420(const QVideoFrame& nv12Frame);
    static QVideoFrame I420ToNV12(webrtc::scoped_refptr<webrtc::I420Buffer> i420Buffer);
};

#endif