#ifndef LOCALVIDEO_H
#define LOCALVIDEO_H

#include "../../../includes.h"

#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QMediaDevices>

class LocalVideo : public QWidget {
    Q_OBJECT

public:
    explicit LocalVideo(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setFixedSize(320, 180);

        videoWidget = new QVideoWidget(this);
        videoWidget->setGeometry(rect());

        camera = new QCamera(QMediaDevices::defaultVideoInput(), this);
        captureSession = new QMediaCaptureSession(this);

        captureSession->setCamera(camera);
        captureSession->setVideoOutput(videoWidget);

        camera->start();
    }

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QWidget::resizeEvent(event);
        if (videoWidget)
            videoWidget->setGeometry(rect());
    }

private:
    QCamera* camera = nullptr;
    QMediaCaptureSession* captureSession = nullptr;
    QVideoWidget* videoWidget = nullptr;
};

#endif