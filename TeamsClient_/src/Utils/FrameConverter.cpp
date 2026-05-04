#include "FrameConverter.h"

#include <libyuv.h>

#include <QDebug>

webrtc::scoped_refptr<webrtc::I420Buffer> FrameConverter::NV12ToI420(const QVideoFrame& nv12Frame) {
  if (!nv12Frame.isValid()) return nullptr;

  QVideoFrame mutableFrame = nv12Frame;  // copie shallow, nécessaire car map() n'est pas const
  if (!mutableFrame.map(QVideoFrame::ReadOnly)) {
    qWarning() << "[FrameConverter] Impossible de mapper la frame NV12";
    return nullptr;
  }

  int width = mutableFrame.width();
  int height = mutableFrame.height();

  auto i420Buffer = webrtc::I420Buffer::Create(width, height);

  const uchar* srcY = mutableFrame.bits(0);
  const uchar* srcUV = mutableFrame.bits(1);
  int srcYStride = mutableFrame.bytesPerLine(0);
  int srcUVStride = mutableFrame.bytesPerLine(1);

  if (!srcY || !srcUV) {
    qWarning() << "[FrameConverter] Pointeurs NV12 null après mapping";
    mutableFrame.unmap();
    return nullptr;
  }

  libyuv::NV12ToI420(srcY, srcYStride, srcUV, srcUVStride, i420Buffer->MutableDataY(),
                     i420Buffer->StrideY(), i420Buffer->MutableDataU(), i420Buffer->StrideU(),
                     i420Buffer->MutableDataV(), i420Buffer->StrideV(), width, height);

  mutableFrame.unmap();
  return i420Buffer;
}

QVideoFrame FrameConverter::I420ToVideoFrame(webrtc::scoped_refptr<webrtc::I420Buffer> i420Buffer) {
  if (!i420Buffer) return QVideoFrame();

  int width = i420Buffer->width();
  int height = i420Buffer->height();

  QVideoFrameFormat format(QSize(width, height), QVideoFrameFormat::Format_NV12);
  QVideoFrame frame(format);

  if (!frame.map(QVideoFrame::WriteOnly)) return QVideoFrame();

  uchar* dstY = frame.bits(0);
  uchar* dstUV = frame.bits(1);
  int dstYStride = frame.bytesPerLine(0);
  int dstUVStride = frame.bytesPerLine(1);

  libyuv::I420ToNV12(i420Buffer->DataY(), i420Buffer->StrideY(), i420Buffer->DataU(),
                     i420Buffer->StrideU(), i420Buffer->DataV(), i420Buffer->StrideV(), dstY,
                     dstYStride, dstUV, dstUVStride, width, height);

  frame.unmap();
  return frame;
}