#include "PConnectionObserver.h"
#include "PConnectionController.h"

PConnectionObserver::PConnectionObserver(PConnectionController* owner) : owner_(owner) {}

void PConnectionObserver::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
  std::string sdp;
  candidate->ToString(&sdp);

  if (owner_->onLocalIce) {
    owner_->onLocalIce(sdp, candidate->sdp_mid(), candidate->sdp_mline_index());
  }
}

void PConnectionObserver::OnConnectionChange(
    webrtc::PeerConnectionInterface::PeerConnectionState state) {
      qDebug() << "[PConnectionObserver] OnConnectionChange state=" << (int)state;
  if (!owner_) return;

  switch (state) {
    case webrtc::PeerConnectionInterface::PeerConnectionState::kConnected:
      // Connexion P2P + DTLS établie → transport utilisable
      owner_->onP2PChange(true);
      break;

    case webrtc::PeerConnectionInterface::PeerConnectionState::kDisconnected:
    case webrtc::PeerConnectionInterface::PeerConnectionState::kFailed:
    case webrtc::PeerConnectionInterface::PeerConnectionState::kClosed:
      owner_->onP2PChange(false);
      break;

    default:
      // kNew / kConnecting → rien à faire
      break;
  }
}

void PConnectionObserver::OnTrack(
    webrtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) {
    auto track = transceiver->receiver()->track();
    if (track && track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind) {
        auto videoTrack = webrtc::scoped_refptr<webrtc::VideoTrackInterface>(
            static_cast<webrtc::VideoTrackInterface*>(track.get()));
        videoTrack->AddOrUpdateSink(
            Sources::instance().remoteVideo().get(),
            webrtc::VideoSinkWants());
    }
}