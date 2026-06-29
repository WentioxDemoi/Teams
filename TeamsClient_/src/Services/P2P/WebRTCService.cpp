#include "WebRTCService.h"
#include "P2P/PConnectionController.h"

#include <QPointer>

WebRTCService::WebRTCService(QObject *parent)
    : QObject(parent), pConnectionController_(std::make_unique<PConnectionController>()) {
  QPointer<WebRTCService> self(this);
}

void WebRTCService::startCall() { pConnectionController_->createOffer(); }
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
  qDebug() << "[WebRTCService] onRemoteIce received, mid=" << mid.size() << "index=" << index;
  pConnectionController_->addIceCandidate(candidate.toStdString(), mid.toStdString(), index);
}

void WebRTCService::setCallBacks(std::function<void(const std::string &sdp)> onLocalOffer,
                  std::function<void(const std::string &sdp)> onLocalAnswer,
                  std::function<void(const std::string &candidate, const std::string &mid, int index)> onLocalIce,
                  std::function<void(bool inProgress)> onP2PChange)
                  {
                    pConnectionController_->onLocalOffer = onLocalOffer;
                    pConnectionController_->onLocalAnswer = onLocalAnswer;
                    pConnectionController_->onLocalIce = onLocalIce;
                    pConnectionController_->onP2PChange = onP2PChange;
                  }