#include "PConnectionController.h"

#include "PConnectionObserver.h"
#include "Sources.h"
#include "WebRTCObservers.h"
#include <QtCore/qlogging.h>

PConnectionController::PConnectionController() {
  // PConnectionController.cpp — constructeur
network_thread_ = webrtc::Thread::CreateWithSocketServer();
network_thread_->Start();

worker_thread_ = webrtc::Thread::Create();
worker_thread_->Start();

signaling_thread_ = webrtc::Thread::Create();
signaling_thread_->Start();

factory_ = webrtc::CreatePeerConnectionFactory(
    network_thread_.get(),
    worker_thread_.get(),
    signaling_thread_.get(),
    nullptr,
    webrtc::CreateBuiltinAudioEncoderFactory(),
    webrtc::CreateBuiltinAudioDecoderFactory(),
    webrtc::CreateBuiltinVideoEncoderFactory(),
    webrtc::CreateBuiltinVideoDecoderFactory(),
    nullptr,
    nullptr);
  RTC_CHECK(factory_) << "Failed to create PeerConnectionFactory";

  webrtc::PeerConnectionInterface::RTCConfiguration config;
  webrtc::PeerConnectionInterface::IceServer stun;
  stun.urls.push_back("stun:stun.l.google.com:19302");
  config.servers.push_back(stun);

  // scoped_refptr obligatoire : WebRTC garde une référence interne sur l'observer
  observer_ = webrtc::scoped_refptr<PConnectionObserver>(new PConnectionObserver(this));
  webrtc::PeerConnectionDependencies deps(observer_.get());
  peer_ = factory_->CreatePeerConnectionOrError(config, std::move(deps)).MoveValue();
  RTC_CHECK(peer_) << "Failed to create PeerConnection";

  auto videoTrack = factory_->CreateVideoTrack(Sources::instance().localVideo(), "video0");
  peer_->AddTrack(videoTrack, {"stream0"});

  
}

void PConnectionController::createOffer() {
    qDebug()  << "[createOffer] called";
    auto obs = webrtc::scoped_refptr<CreateSdpObserver>(
        new CreateSdpObserver([this](const std::string& sdp) {
            qDebug()  << "[createOffer] OnSuccess, sdp size=" << sdp.size();
            peer_->SetLocalDescription(webrtc::CreateSessionDescription(webrtc::SdpType::kOffer, sdp),
            webrtc::scoped_refptr<SetLocalSdpObserver>(new SetLocalSdpObserver()));
            if (onLocalOffer) {
                qDebug()  << "[createOffer] calling onLocalOffer";
                onLocalOffer(sdp);
            } else {
                RTC_LOG(LS_ERROR) << "[createOffer] onLocalOffer is NULL";
            }
        }));
    peer_->CreateOffer(obs.get(), {});
    qDebug()  << "[createOffer] CreateOffer submitted";
}

void PConnectionController::createAnswer() {
  auto obs = webrtc::scoped_refptr<CreateSdpObserver>(
      new CreateSdpObserver([this](const std::string& sdp) {
        peer_->SetLocalDescription(
            webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, sdp),
            webrtc::scoped_refptr<SetLocalSdpObserver>(new SetLocalSdpObserver()));
        if (onLocalAnswer) onLocalAnswer(sdp);
      }));
  peer_->CreateAnswer(obs.get(), {});
}

void PConnectionController::setRemoteOffer(const std::string& sdp) {
  peer_->SetRemoteDescription(
      webrtc::CreateSessionDescription(webrtc::SdpType::kOffer, sdp),
      webrtc::scoped_refptr<SetRemoteSdpObserver>(new SetRemoteSdpObserver()));
}

void PConnectionController::setRemoteAnswer(const std::string& sdp) {
  peer_->SetRemoteDescription(
      webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, sdp),
      webrtc::scoped_refptr<SetRemoteSdpObserver>(new SetRemoteSdpObserver()));
}

void PConnectionController::addIceCandidate(const std::string& candidate, const std::string& mid,
                                            int index) {
  webrtc::SdpParseError error;
    auto ice = webrtc::CreateIceCandidate(mid, index, candidate, &error);
    if (ice) {
        qDebug() << "[ICE] Adding candidate, mid=" 
                 << QString::fromStdString(mid);
        peer_->AddIceCandidate(ice);
    } else {
        qDebug() << "[ICE] Parse error:" 
                 << QString::fromStdString(error.description);
    }
}

void PConnectionController::close() {
  if (peer_) peer_->Close();
}