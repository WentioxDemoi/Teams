// RemoteVideo.h
#ifndef REMOTEVIDEO_H
#define REMOTEVIDEO_H
#include "../../../Services/P2P/Sources.h"
#include <QVideoWidget>
#include <QVideoFrame>
#include <QMetaObject>
#include <Qt>
#include <api/video/i420_buffer.h>
#include <api/video/video_frame.h>

class RemoteVideo : public QVideoWidget {
    Q_OBJECT
public:
    explicit RemoteVideo(QWidget* parent = nullptr);

public slots:
    void onP2PChange(bool inProgress) {
        if (!inProgress)
            Sources::instance().remoteVideo()->onFrame = nullptr;
    }
};
#endif