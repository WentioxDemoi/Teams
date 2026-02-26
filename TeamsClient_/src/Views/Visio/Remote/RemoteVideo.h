#ifndef REMOTEVIDEO_H
#define REMOTEVIDEO_H
#include "../../../includes.h"
#include "../../../webrtc_includes.h"
#include "../../../Services/Sources.h"
#include "../../../Utils/FrameConverter.h"

class RemoteVideo : public QWidget {
    Q_OBJECT
public:
    explicit RemoteVideo(QWidget* parent = nullptr) : QWidget(parent) {
        setFixedSize(320, 180);
        videoWidget_ = new QVideoWidget(this);
        videoWidget_->setGeometry(rect());
        videoWidget_->show();

        // Abonnement au callback WebRTC → thread Qt via invokeMethod
        Sources::instance().remoteVideo()->onFrame = [this](const webrtc::VideoFrame& frame) {
            auto i420 = frame.video_frame_buffer()->ToI420();
            QVideoFrame qFrame = FrameConverter::I420ToNV12(
                webrtc::scoped_refptr<webrtc::I420Buffer>(
                    static_cast<webrtc::I420Buffer*>(i420.get())));
            QMetaObject::invokeMethod(this, [this, qFrame]() {
                onFrameReady(qFrame);
            }, Qt::QueuedConnection);
        };

        qDebug() << "RemoteVideo started";
    }

public slots:
    void onP2PChange(bool inProgress) {
        if (!inProgress) {
            Sources::instance().remoteVideo()->onFrame = nullptr;
        }
    }

private:
    void onFrameReady(const QVideoFrame& frame) {
        if (videoWidget_ && videoWidget_->videoSink())
            videoWidget_->videoSink()->setVideoFrame(frame);
    }

    QVideoWidget* videoWidget_ = nullptr;
};
#endif