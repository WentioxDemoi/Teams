#include "WebRTCService.h"
#include <QtCore/qcoreapplication.h>


// WebRTCService.cpp (extrait constructeur)

WebRTCService::WebRTCService(QObject* parent)
    : QObject(parent),
      signalingClient_(new SignalingClient(this)),
      pConnectionController_(std::make_unique<PConnectionController>())
{
    // --- PC → Signaling ---
    pConnectionController_->onLocalOffer = [this](const std::string& sdp) {
        signalingClient_->sendOffer(QString::fromStdString(sdp));
    };

    pConnectionController_->onLocalAnswer = [this](const std::string& sdp) {
        signalingClient_->sendAnswer(QString::fromStdString(sdp));
    };

    pConnectionController_->onLocalIce = [this](const std::string& c,
                             const std::string& mid,
                             int index) {
        signalingClient_->sendIce(QString::fromStdString(c));
    };

    pConnectionController_->onP2PChange = [this](bool inProgress) {
    QMetaObject::invokeMethod(
        this,
        [this, inProgress]() {
            emit onP2PChange(inProgress);
        },
        Qt::QueuedConnection);
};

    // --- Signaling → PC ---
    connect(signalingClient_, &SignalingClient::offerReceived,
            this, &WebRTCService::onRemoteOffer);

    connect(signalingClient_, &SignalingClient::answerReceived,
            this, &WebRTCService::onRemoteAnswer);

    connect(signalingClient_, &SignalingClient::iceReceived,
            this, &WebRTCService::onRemoteIce);
    // connect(this, &WebRTCService::onRemoteTrack, remoteVideo_, &RemoteVideo::attachTrack);
}


  void WebRTCService::startCall() {
    pConnectionController_->createOffer();
  }
  void WebRTCService::acceptCall() {
    pConnectionController_->createAnswer();
  }
  void WebRTCService::hangup() {
    pConnectionController_->close();
  }

  void WebRTCService::onRemoteOffer(QString sdp) {
    pConnectionController_->setRemoteOffer(sdp.toStdString());
  }

  void WebRTCService::onRemoteAnswer(QString sdp) {
    pConnectionController_->setRemoteAnswer(sdp.toStdString());
  }

  void WebRTCService::onRemoteIce(QString candidate, QString mid, int index) {
    pConnectionController_->addIceCandidate(candidate.toStdString(), mid.toStdString(), index);
  }

  void WebRTCService::disconnectFromServer()
  {
    //Appeler le disco de signaling qui appelera celui de network.
  }