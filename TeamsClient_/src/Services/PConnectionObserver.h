#ifndef PCONNECTIONOBSERVER_H
#define PCONNECTIONOBSERVER_H

#include "../includes.h"
#include "../webrtc_includes.h"

class PConnectionController;  // FD

class PConnectionObserver
    : public webrtc::PeerConnectionObserver,
      public webrtc::RefCountInterface  // Compteur de référence, code legacy et façon de faire
                                        // legacy de webrtc, maintenant on a les shared_ptr.
{
 public:
  explicit PConnectionObserver(PConnectionController* owner);

  // PeerConnectionObserver
  void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
  void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState) override {}
  void OnDataChannel(webrtc::scoped_refptr<webrtc::DataChannelInterface>) override {}
  void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState) override {}
  void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState) override {}
  void OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState) override;
  void OnTrack(webrtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) override;

  WEBRTC_REF_COUNT_IMPL // Aller voir 

 private:
  PConnectionController* owner_;
};

#endif