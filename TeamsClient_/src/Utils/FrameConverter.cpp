#include "FrameConverter.h"

webrtc::scoped_refptr<webrtc::I420Buffer> FrameConverter::NV12ToI420(const QVideoFrame& nv12Frame)
{
    if (!nv12Frame.isValid())
        return nullptr;

    // ✅ OBLIGATOIRE : mapper la frame avant d'accéder aux bits
    QVideoFrame mutableFrame = nv12Frame; // copie shallow, nécessaire car map() n'est pas const
    if (!mutableFrame.map(QVideoFrame::ReadOnly)) {
        qWarning() << "[FrameConverter] Impossible de mapper la frame NV12";
        return nullptr;
    }

    int width  = mutableFrame.width();
    int height = mutableFrame.height();

    auto i420Buffer = webrtc::I420Buffer::Create(width, height);

    const uchar* srcY  = mutableFrame.bits(0);
    const uchar* srcUV = mutableFrame.bits(1);
    int srcYStride  = mutableFrame.bytesPerLine(0);
    int srcUVStride = mutableFrame.bytesPerLine(1);

    // ✅ Vérifier que les pointeurs sont valides après mapping
    if (!srcY || !srcUV) {
        qWarning() << "[FrameConverter] Pointeurs NV12 null après mapping";
        mutableFrame.unmap();
        return nullptr;
    }

    libyuv::NV12ToI420(
        srcY, srcYStride,
        srcUV, srcUVStride,
        i420Buffer->MutableDataY(), i420Buffer->StrideY(),
        i420Buffer->MutableDataU(), i420Buffer->StrideU(),
        i420Buffer->MutableDataV(), i420Buffer->StrideV(),
        width, height
    );

    mutableFrame.unmap(); // ✅ Toujours unmap après utilisation
    return i420Buffer;
}

// QVideoFrame FrameConverter::I420ToNV12(webrtc::scoped_refptr<webrtc::I420Buffer> i420Buffer)
// {
//     if (!i420Buffer) {
//         qDebug() << "[FrameConverter] buffer is null";
//         return QVideoFrame();
//     }

//     int width  = i420Buffer->width();
//     int height = i420Buffer->height();
//     qDebug() << "[FrameConverter] size=" << width << "x" << height;

//     // Fallback via QImage RGB32 — pas optimal mais fonctionne sur toutes versions Qt 6.x
//     QImage img(width, height, QImage::Format_RGB32);

//     libyuv::I420ToARGB(
//         i420Buffer->DataY(), i420Buffer->StrideY(),
//         i420Buffer->DataU(), i420Buffer->StrideU(),
//         i420Buffer->DataV(), i420Buffer->StrideV(),
//         img.bits(), img.bytesPerLine(),
//         width, height
//     );

//     QVideoFrame frame(img);
//     qDebug() << "[FrameConverter] frame valid=" << frame.isValid()
//              << " format=" << frame.pixelFormat();
//     return frame;
// }

// Dans FrameConverter — retourne QImage directement
// QImage FrameConverter::I420ToQImage(webrtc::scoped_refptr<webrtc::I420Buffer> i420Buffer) {
//     int width  = i420Buffer->width();
//     int height = i420Buffer->height();
//     QImage img(width, height, QImage::Format_RGB32);

//     // ✅ ABGR correspond au Format_RGB32 de Qt (BGRX en mémoire, little-endian)
//     libyuv::I420ToABGR(
//         i420Buffer->DataY(), i420Buffer->StrideY(),
//         i420Buffer->DataU(), i420Buffer->StrideU(),
//         i420Buffer->DataV(), i420Buffer->StrideV(),
//         img.bits(), img.bytesPerLine(),
//         width, height
//     );
//     return img;
// }

// FrameConverter.cpp
QVideoFrame FrameConverter::I420ToVideoFrame(webrtc::scoped_refptr<webrtc::I420Buffer> i420Buffer)
{
    if (!i420Buffer)
        return QVideoFrame();

    int width  = i420Buffer->width();
    int height = i420Buffer->height();

    // Allouer un QVideoFrame NV12 directement (pas de QImage intermédiaire)
    QVideoFrameFormat format(QSize(width, height), QVideoFrameFormat::Format_NV12);
    QVideoFrame frame(format);

    if (!frame.map(QVideoFrame::WriteOnly))
        return QVideoFrame();

    uchar* dstY  = frame.bits(0);
    uchar* dstUV = frame.bits(1);
    int dstYStride  = frame.bytesPerLine(0);
    int dstUVStride = frame.bytesPerLine(1);

    libyuv::I420ToNV12(
        i420Buffer->DataY(), i420Buffer->StrideY(),
        i420Buffer->DataU(), i420Buffer->StrideU(),
        i420Buffer->DataV(), i420Buffer->StrideV(),
        dstY,  dstYStride,
        dstUV, dstUVStride,
        width, height
    );

    frame.unmap();
    return frame;
}