#include "PConnectionController.h"

#include "Sources.h"
#include "api/create_peerconnection_factory.h"
#include "audio_codecs/builtin_audio_decoder_factory.h"
#include "audio_codecs/builtin_audio_encoder_factory.h"
#include "video_codecs/builtin_video_decoder_factory.h"
#include "video_codecs/builtin_video_encoder_factory.h"

PConnectionController::PConnectionController() {
  network_thread_ = webrtc::Thread::CreateWithSocketServer();
  network_thread_->Start();

  worker_thread_ = webrtc::Thread::Create();
  worker_thread_->Start();

  signaling_thread_ = webrtc::Thread::Create();
  signaling_thread_->Start();

  factory_ = webrtc::CreatePeerConnectionFactory(
      network_thread_.get(), worker_thread_.get(), signaling_thread_.get(), nullptr,
      webrtc::CreateBuiltinAudioEncoderFactory(), webrtc::CreateBuiltinAudioDecoderFactory(),
      webrtc::CreateBuiltinVideoEncoderFactory(), webrtc::CreateBuiltinVideoDecoderFactory(), nullptr, nullptr);
  RTC_CHECK(factory_) << "[PConnectionController] Failed to create PeerConnectionFactory";

  webrtc::PeerConnectionInterface::RTCConfiguration config;
  webrtc::PeerConnectionInterface::IceServer stun;
  stun.urls.push_back("stun:stun.l.google.com:19302");
  config.servers.push_back(stun);

  // scoped_refptr obligatoire : WebRTC garde une référence interne sur l'observer
  observer_ = webrtc::scoped_refptr<PConnectionObserver>(new PConnectionObserver(this));
  webrtc::PeerConnectionDependencies deps(observer_.get());
  peer_ = factory_->CreatePeerConnectionOrError(config, std::move(deps)).MoveValue();
  RTC_CHECK(peer_) << "[PConnectionController] Failed to create PeerConnection";

  auto videoTrack = factory_->CreateVideoTrack(Sources::instance().localVideo(), "video0");
  peer_->AddTrack(videoTrack, {"stream0"});

  // Audio : la capture micro est gérée en interne par l'AudioDeviceModule
  // (créé implicitement puisqu'on a passé nullptr en 4e argument de
  // CreatePeerConnectionFactory), pas besoin de pousser des frames nous-mêmes.
  webrtc::scoped_refptr<webrtc::AudioSourceInterface> audioSource = factory_->CreateAudioSource(webrtc::AudioOptions());
  audioTrack_ = factory_->CreateAudioTrack("audio0", audioSource.get());
  auto result = peer_->AddTrack(audioTrack_, {"stream0"});
  if (result.ok()) {
    audioSender_ = result.value();
  } else {
    RTC_LOG(LS_ERROR) << "[PConnectionController] Failed to add audio track";
  }
}

void PConnectionController::createOffer() {
  qDebug() << "[createOffer] called";
  auto obs = webrtc::scoped_refptr<CreateSdpObserver>(new CreateSdpObserver([this](const std::string &sdp) {
    qDebug() << "[createOffer] OnSuccess, sdp size=" << sdp.size();
    peer_->SetLocalDescription(webrtc::CreateSessionDescription(webrtc::SdpType::kOffer, sdp),
                               webrtc::scoped_refptr<SetLocalSdpObserver>(new SetLocalSdpObserver()));
    if (onLocalOffer) {
      qDebug() << "[createOffer] calling onLocalOffer";
      onLocalOffer(sdp);
    } else {
      RTC_LOG(LS_ERROR) << "[createOffer] onLocalOffer is NULL";
    }
  }));
  peer_->CreateOffer(obs.get(), {});
  qDebug() << "[createOffer] CreateOffer submitted";
}

void PConnectionController::createAnswer() {
  auto obs = webrtc::scoped_refptr<CreateSdpObserver>(new CreateSdpObserver([this](const std::string &sdp) {
    peer_->SetLocalDescription(webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, sdp),
                               webrtc::scoped_refptr<SetLocalSdpObserver>(new SetLocalSdpObserver()));
    if (onLocalAnswer)
      onLocalAnswer(sdp);
  }));
  peer_->CreateAnswer(obs.get(), {});
}

void PConnectionController::setRemoteOffer(const std::string &sdp) {
  peer_->SetRemoteDescription(webrtc::CreateSessionDescription(webrtc::SdpType::kOffer, sdp),
                              webrtc::scoped_refptr<SetRemoteSdpObserver>(new SetRemoteSdpObserver()));
}

void PConnectionController::setRemoteAnswer(const std::string &sdp) {
  peer_->SetRemoteDescription(webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, sdp),
                              webrtc::scoped_refptr<SetRemoteSdpObserver>(new SetRemoteSdpObserver()));
}

void PConnectionController::addIceCandidate(const std::string &candidate, const std::string &mid, int index) {
  webrtc::SdpParseError error;
  auto ice = webrtc::CreateIceCandidate(mid, index, candidate, &error);
  if (ice) {
    qDebug() << "[ICE] Adding candidate, mid=" << QString::fromStdString(mid);
    peer_->AddIceCandidate(ice);
  } else {
    qDebug() << "[ICE] Parse error:" << QString::fromStdString(error.description);
  }
}

// Ici c'est pas bon, on ne respecte pas la règle des 5. Lorsqu'on déclare un destructeur, on doit tout déclarer.
// SOlution, suprimer le destructeur et ré-organiser dans le .h les variables pour que l'ordre de destrcution soit bon
PConnectionController::~PConnectionController() {
  if (peer_) {
    if (audioSender_) {
      peer_->RemoveTrackOrError(audioSender_);
      audioSender_ = nullptr;
    }
    peer_->Close();
    peer_ = nullptr;
  }
  audioTrack_ = nullptr;
  observer_ = nullptr;
  factory_ = nullptr;

  if (signaling_thread_) signaling_thread_->Stop();
  if (worker_thread_) worker_thread_->Stop();
  if (network_thread_) network_thread_->Stop();
}

void PConnectionController::close() {
  if (peer_)
    peer_->Close();
}