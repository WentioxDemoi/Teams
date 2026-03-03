#ifndef LOCALVIDEO_H
#define LOCALVIDEO_H

#include "../../../Services/P2P/LocalVideoSource.h"

#include <QWidget>
#include <QVideoSink>
#include <QVideoWidget>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QVideoFrame>
#include <QVideoFrameFormat>
#include <QDebug>
#include <api/scoped_refptr.h>
#include <api/video/i420_buffer.h>
#include <api/video/video_frame.h> 

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