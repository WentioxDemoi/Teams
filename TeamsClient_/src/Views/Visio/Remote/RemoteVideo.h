#ifndef REMOTEVIDEO_H
#define REMOTEVIDEO_H
#include "../../../includes.h"
#include "../../../webrtc_includes.h"
#include "../../../Services/Sources.h"
#include <QPainter>
#include "../../../Utils/FrameConverter.h"

class RemoteVideo : public QWidget {
    Q_OBJECT
public:
    explicit RemoteVideo(QWidget* parent = nullptr) : QWidget(parent) {
        setFixedSize(320, 180);

        Sources::instance().remoteVideo()->onFrame = [this](const webrtc::VideoFrame& frame) {
            auto i420Interface = frame.video_frame_buffer()->ToI420();

            auto i420Buffer = webrtc::I420Buffer::Create(
                i420Interface->width(),
                i420Interface->height()
            );
            i420Buffer->CropAndScaleFrom(*i420Interface);

            // Conversion sur le thread Qt, scoped_refptr est thread-safe
            QMetaObject::invokeMethod(this, [this, i420Buffer]() {
                QImage img = FrameConverter::I420ToQImage(i420Buffer);
onFrameReady(img);
            }, Qt::QueuedConnection);
        };
    }

public slots:
    void onP2PChange(bool inProgress) {
        if (!inProgress)
            Sources::instance().remoteVideo()->onFrame = nullptr;
    }

protected:
    // paintEvent
void paintEvent(QPaintEvent*) override {
    QPainter painter(this);
    if (!currentImage_.isNull()) {
        painter.drawImage(rect(), currentImage_);
    } else {
        painter.fillRect(rect(), Qt::black);
    }
}

private:
QImage currentImage_; 
    void onFrameReady(const QImage& img) {
    currentImage_ = img;
    update();
}

    QVideoFrame currentFrame_;
};
#endif