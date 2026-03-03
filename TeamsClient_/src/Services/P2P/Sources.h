#ifndef SOURCES_H
#define SOURCES_H

#include "LocalVideoSource.h"
#include "RemoteVideoSource.h"

/**
 * @class Sources
 * @brief Gestionnaire singleton des sources vidéo.
 *
 * Fournit un accès global aux sources vidéo locales et distantes,
 * garantissant qu'une seule instance de chaque source est utilisée dans l'application.
 */
class Sources {
 public:
  static Sources& instance();

  webrtc::scoped_refptr<LocalVideoSource> localVideo();
  webrtc::scoped_refptr<RemoteVideoSource> remoteVideo();

  Sources(const Sources&) = delete;
  Sources& operator=(const Sources&) = delete;

 private:
  Sources();
  ~Sources() = default;

  webrtc::scoped_refptr<LocalVideoSource> local_video_;
  webrtc::scoped_refptr<RemoteVideoSource> remote_video_;
};

#endif