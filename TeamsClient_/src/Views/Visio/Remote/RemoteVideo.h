// RemoteVideo.h
#ifndef REMOTEVIDEO_H
#define REMOTEVIDEO_H
#include "../../../includes.h"
#include "../../../webrtc_includes.h"
#include "../../../Services/P2P/Sources.h"
#include "../../../Utils/FrameConverter.h"
#include "../../../Services/P2P/RemoteVideoSource.h"


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