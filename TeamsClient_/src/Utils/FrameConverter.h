#ifndef FRAMECONVERTER_H
#define FRAMECONVERTER_H

#include "../webrtc_includes.h"
#include "api/video/i420_buffer.h"
#include "scoped_refptr.h"
#include <QVideoFrame>

/**
 * @class FrameConverter
 * @brief Convertisseur de formats vidéo.
 *
 * Fournit des fonctions statiques pour convertir des frames vidéo entre
 * le format NV12 (utilisé par Qt) et le format I420 (utilisé par WebRTC),
 * facilitant l'intégration des flux vidéo locaux et distants.
 */
class FrameConverter {
 public:
  static webrtc::scoped_refptr<webrtc::I420Buffer> NV12ToI420(const QVideoFrame& nv12Frame);
  static QVideoFrame I420ToVideoFrame(webrtc::scoped_refptr<webrtc::I420Buffer> i420Buffer);
};

#endif