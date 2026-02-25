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