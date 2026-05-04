#ifndef REMOTEVIDEO_H
#define REMOTEVIDEO_H
#include <api/video/i420_buffer.h>
#include <api/video/video_frame.h>

#include <QMetaObject>
#include <QVideoFrame>
#include <QVideoWidget>
#include <Qt>

#include "../../../Services/P2P/Sources.h"

/**
 * @class RemoteVideo
 * @brief Widget d'affichage de la vidéo distante.
 *
 * Affiche le flux vidéo provenant d'un pair distant.
 */
class RemoteVideo : public QVideoWidget {
  Q_OBJECT
 public:
  explicit RemoteVideo(QWidget* parent = nullptr);

 public slots:
  void onP2PChange(bool inProgress) {
    if (!inProgress) Sources::instance().remoteVideo()->onFrame = nullptr;
  }
};
#endif