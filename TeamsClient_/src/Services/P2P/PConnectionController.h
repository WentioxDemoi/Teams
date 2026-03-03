#ifndef PCONNECTIONCONTROLLER_H
#define PCONNECTIONCONTROLLER_H

#include "PConnectionObserver.h"
#include "WebRTCObservers.h"
#include "rtc_base/thread.h"

/**
 * @class PConnectionController
 * @brief Contrôleur d'une connexion WebRTC.
 *
 * Gère la création d'offres et de réponses, l'ajout de candidats ICE,
 * et la fermeture d'une PeerConnection. Fournit des callbacks pour notifier
 * les changements d'état P2P et la disponibilité des informations locales.
 */
class PConnectionController {
 public:
  PConnectionController();

  void createOffer();
  void createAnswer();
  void setRemoteOffer(const std::string& sdp);
  void setRemoteAnswer(const std::string& sdp);
  void addIceCandidate(const std::string& candidate, const std::string& mid, int index);
  void close();

  std::function<void(const std::string& sdp)> onLocalOffer;
  std::function<void(const std::string& sdp)> onLocalAnswer;
  std::function<void(const std::string& candidate, const std::string& mid, int index)> onLocalIce;
  std::function<void(bool inProgress)> onP2PChange;

 private:
  webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory_;
  webrtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_;
  webrtc::scoped_refptr<PConnectionObserver> observer_;

  std::unique_ptr<webrtc::Thread> network_thread_;
  std::unique_ptr<webrtc::Thread> worker_thread_;
  std::unique_ptr<webrtc::Thread> signaling_thread_;
};

#endif