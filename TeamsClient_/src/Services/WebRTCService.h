#ifndef WEBRTCSERVICE_H
#define WEBRTCSERVICE_H

#include "../webrtc_includes.h" 
#include "../includes.h"
#include "NetworkService.h"
#include "PConnectionController.h"
#include "SignalingClient.h"
#include <memory>

class WebRTCService : public QObject {
    Q_OBJECT
public:
    explicit WebRTCService(QObject* parent = nullptr);
    void disconnectFromServer();

public slots:
    void startCall();
    void acceptCall();
    void hangup();

private slots:
    void onRemoteOffer(QString sdp);
    void onRemoteAnswer(QString sdp);
    void onRemoteIce(QString candidate, QString mid, int index);

signals:
    void connected();
    void disconnected();
    void remoteVideoFrame(const webrtc::VideoFrame&);
    void error(QString);

private:
    std::unique_ptr<PConnectionController> pConnectionController_;
    SignalingClient *signalingClient_;

};
#endif


