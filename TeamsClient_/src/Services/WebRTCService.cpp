#include <QtCore/qcoreapplication.h>

#include "AuthService.h"
#include "Interfaces/IAuthService.h"
#include "ServiceLocator.h"
#include "SignalingClient.h"
#include "WebRTCService.h"

// WebRTCService.cpp (extrait constructeur)

WebRTCService::WebRTCService(QObject* parent)
    : QObject(parent),
      signalingClient_(new SignalingClient(this)),
      pConnectionController_(std::make_unique<PConnectionController>()) {
  // --- PC → Signaling ---
  pConnectionController_->onLocalOffer = [this](const std::string& sdp) {
    qDebug() << "[WebRTCService] onLocalOffer triggered, sending offer";
    signalingClient_->sendOffer(QString::fromStdString(sdp));
};

  pConnectionController_->onLocalAnswer = [this](const std::string& sdp) {
    qDebug() << "[WebRTCService] onP2PChange:";
    signalingClient_->sendAnswer(QString::fromStdString(sdp));
  };

  pConnectionController_->onLocalIce = [this](const std::string& c, const std::string& mid,
                                              int index) {
    QJsonObject ice;
    ice["candidate"] = QString::fromStdString(c);
    ice["mid"] = QString::fromStdString(mid);
    ice["index"] = index;

    const QString payload = QString::fromUtf8(QJsonDocument(ice).toJson(QJsonDocument::Compact));
    signalingClient_->sendIce(payload);
  };

  pConnectionController_->onP2PChange = [this](bool inProgress) {
    qDebug() << "[WebRTCService] onP2PChange connected=" << inProgress;
    QMetaObject::invokeMethod(
        this, [this, inProgress]() { emit onP2PChange(inProgress); }, Qt::QueuedConnection);
  };

  // --- Signaling → PC ---
  connect(signalingClient_, &SignalingClient::offerReceived, this, &WebRTCService::onRemoteOffer);

  connect(signalingClient_, &SignalingClient::answerReceived, this, &WebRTCService::onRemoteAnswer);

  connect(signalingClient_, &SignalingClient::iceReceived, this, &WebRTCService::onRemoteIce);
  connect(this, &WebRTCService::registerWithServer4WebRTC, signalingClient_, &SignalingClient::registerWithServer4WebRTC);
  
}

void WebRTCService::startCall() {
    qDebug() << "[WebRTCService] startCall()";
    pConnectionController_->createOffer();
}
void WebRTCService::acceptCall() { pConnectionController_->createAnswer(); }
void WebRTCService::hangup() { pConnectionController_->close(); }

void WebRTCService::onRemoteOffer(QString sdp) {
    qDebug() << "[WebRTCService] onRemoteOffer received, sdp size=" << sdp.size();
    pConnectionController_->setRemoteOffer(sdp.toStdString());
    pConnectionController_->createAnswer();
}
void WebRTCService::onRemoteAnswer(QString sdp) {
    qDebug() << "[WebRTCService] onRemoteAnswer received, sdp size=" << sdp.size();
    pConnectionController_->setRemoteAnswer(sdp.toStdString());
}
void WebRTCService::onRemoteIce(QString candidate, QString mid, int index) {
    qDebug() << "[WebRTCService] onRemoteIce received, mid=" << mid << "index=" << index;
    pConnectionController_->addIceCandidate(candidate.toStdString(), mid.toStdString(), index);
}
void WebRTCService::disconnectFromServer() {
  // Appeler le disco de signaling qui appelera celui de network.
}