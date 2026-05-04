#ifndef PCONNECTIONOBSERVER_H
#define PCONNECTIONOBSERVER_H

#include "../../webrtc_includes.h"
#include "api/peer_connection_interface.h"
#include "api/ref_count.h"
#include "api/data_channel_interface.h"
#include "api/rtp_transceiver_interface.h"
#include <QDebug>

class PConnectionController;  // FD

/**
 * @class PConnectionObserver
 * @brief Observateur pour une connexion WebRTC.
 *
 * Surveille les événements d'une PeerConnection, notamment les candidats ICE, 
 * les changements de connexion et les flux médias entrants, en relayant les informations 
 * vers le contrôleur de connexion associé.
 */
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
  void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState state) override {qDebug() << "[PConnectionObserver] OnIceGatheringChange state=" << (int)state;}
  void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState) override {}
  void OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState) override;
  void OnTrack(webrtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) override;

  WEBRTC_REF_COUNT_IMPL // Aller voir 

 private:
  PConnectionController* owner_;
};

#endif