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
  ~PConnectionController();

  void createOffer();
  void createAnswer();
  void setRemoteOffer(const std::string &sdp);
  void setRemoteAnswer(const std::string &sdp);
  void addIceCandidate(const std::string &candidate, const std::string &mid, int index);
  void close();

  std::function<void(const std::string &sdp)> onLocalOffer;
  std::function<void(const std::string &sdp)> onLocalAnswer;
  std::function<void(const std::string &candidate, const std::string &mid, int index)> onLocalIce;
  std::function<void(bool isConnected)> isContactConnectedChanged;

  void setMicEnabled(bool enabled) {
    if (audioTrack_)
      audioTrack_->set_enabled(enabled);
  }

private:
  // IMPORTANT — ordre de déclaration volontaire.
  // En C++, les membres sont détruits dans l'ordre INVERSE de leur déclaration.
  // peer_/observer_/factory_ ont besoin que les threads WebRTC soient encore
  // vivants pour se détruire proprement (le proxy interne de PeerConnection
  // poste sur signaling_thread_ lors de sa destruction). Si les threads sont
  // arrêtés/détruits avant peer_, on obtient un accès mémoire invalide
  // (EXC_BAD_ACCESS dans ~PeerConnectionProxyWithInternal()).
  //
  // Threads déclarés EN PREMIER → détruits EN DERNIER.
  std::unique_ptr<webrtc::Thread> network_thread_;
  std::unique_ptr<webrtc::Thread> worker_thread_;
  std::unique_ptr<webrtc::Thread> signaling_thread_;

  // factory_ détruite avant les threads, après peer_/observer_.
  webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory_;

  webrtc::scoped_refptr<webrtc::AudioTrackInterface> audioTrack_;

  // observer_ et peer_ déclarés EN DERNIER → détruits EN PREMIER, threads encore vivants.
  webrtc::scoped_refptr<PConnectionObserver> observer_;
  webrtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_;
};

#endif