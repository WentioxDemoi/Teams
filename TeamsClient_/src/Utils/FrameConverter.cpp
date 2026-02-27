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
    if (!i420Buffer) {
        qDebug() << "[FrameConverter] buffer is null";
        return QVideoFrame();
    }

    int width  = i420Buffer->width();
    int height = i420Buffer->height();
    qDebug() << "[FrameConverter] size=" << width << "x" << height;

    // Fallback via QImage RGB32 — pas optimal mais fonctionne sur toutes versions Qt 6.x
    QImage img(width, height, QImage::Format_RGB32);

    libyuv::I420ToARGB(
        i420Buffer->DataY(), i420Buffer->StrideY(),
        i420Buffer->DataU(), i420Buffer->StrideU(),
        i420Buffer->DataV(), i420Buffer->StrideV(),
        img.bits(), img.bytesPerLine(),
        width, height
    );

    QVideoFrame frame(img);
    qDebug() << "[FrameConverter] frame valid=" << frame.isValid()
             << " format=" << frame.pixelFormat();
    return frame;
}

// Dans FrameConverter — retourne QImage directement
QImage FrameConverter::I420ToQImage(webrtc::scoped_refptr<webrtc::I420Buffer> i420Buffer) {
    int width  = i420Buffer->width();
    int height = i420Buffer->height();
    QImage img(width, height, QImage::Format_RGB32);
    libyuv::I420ToARGB(
        i420Buffer->DataY(), i420Buffer->StrideY(),
        i420Buffer->DataU(), i420Buffer->StrideU(),
        i420Buffer->DataV(), i420Buffer->StrideV(),
        img.bits(), img.bytesPerLine(),
        width, height
    );
    return img;
}