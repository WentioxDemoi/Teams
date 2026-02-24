#include "FrameConverter.h"

webrtc::scoped_refptr<webrtc::I420Buffer> FrameConverter::NV12ToI420(const QVideoFrame& nv12Frame)
{
    if (!nv12Frame.isValid())
        return nullptr;

    int width  = nv12Frame.width();
    int height = nv12Frame.height();

    auto i420Buffer = webrtc::I420Buffer::Create(width, height);

    const uchar* srcY  = nv12Frame.bits(0);
    const uchar* srcUV = nv12Frame.bits(1);

    int srcYStride  = nv12Frame.bytesPerLine(0);
    int srcUVStride = nv12Frame.bytesPerLine(1);

    libyuv::NV12ToI420(
        srcY, srcYStride,
        srcUV, srcUVStride,
        i420Buffer->MutableDataY(), i420Buffer->StrideY(),
        i420Buffer->MutableDataU(), i420Buffer->StrideU(),
        i420Buffer->MutableDataV(), i420Buffer->StrideV(),
        width, height
    );

    return i420Buffer;
}

QVideoFrame FrameConverter::I420ToNV12(webrtc::scoped_refptr<webrtc::I420Buffer> i420Buffer)
{
    if (!i420Buffer)
        return QVideoFrame();

    int width  = i420Buffer->width();
    int height = i420Buffer->height();

    QVideoFrameFormat fmt(QSize(width, height), QVideoFrameFormat::Format_NV12);
    QVideoFrame nv12Frame(fmt);

    if (!nv12Frame.map(QVideoFrame::WriteOnly))
        return QVideoFrame();

    uchar* dstY  = nv12Frame.bits(0);
    uchar* dstUV = nv12Frame.bits(1);

    int dstYStride  = nv12Frame.bytesPerLine(0);
    int dstUVStride = nv12Frame.bytesPerLine(1);

    libyuv::I420ToNV12(
        i420Buffer->DataY(), i420Buffer->StrideY(),
        i420Buffer->DataU(), i420Buffer->StrideU(),
        i420Buffer->DataV(), i420Buffer->StrideV(),
        dstY, dstYStride,
        dstUV, dstUVStride,
        width, height
    );

    nv12Frame.unmap();
    return nv12Frame;
}