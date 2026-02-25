#ifndef LOCALVIDEO_H
#define LOCALVIDEO_H

#include "../../../includes.h"
#include "../../../Utils/FrameConverter.h"
#include "../../../Services/Sources.h"
#include "LocalVideoSource.h"
#include <scoped_refptr.h>

class LocalVideo : public QWidget {
    Q_OBJECT

public:
    explicit LocalVideo(QWidget *parent = nullptr);

private slots:
    void OnFrameChanged(const QVideoFrame &frame);

public slots:
    void OnP2PChange(bool inProgress);

protected:

private:
    QVideoSink *sink = nullptr;
    QCamera* camera = nullptr;
    QMediaCaptureSession* captureSession = nullptr;
    QVideoWidget* videoWidget = nullptr;
    bool p2pInProgress = false;
    webrtc::scoped_refptr<LocalVideoSource> videoSource;
};

#endif