#ifndef PCONNECTIONCONTROLLER_H
#define PCONNECTIONCONTROLLER_H

#include "../includes.h"
#include "../webrtc_includes.h"
#include "PConnectionObserver.h"
#include "Sources.h"

class PConnectionController {
 public:
  PConnectionController();

  // --- actions ---
  void createOffer();
  void createAnswer();
  void setRemoteOffer(const std::string& sdp);
  void setRemoteAnswer(const std::string& sdp);
  void addIceCandidate(const std::string& candidate, const std::string& mid, int index);
  void close();

  // --- callbacks vers WebRTCService ---
  std::function<void(const std::string& sdp)> onLocalOffer;
  std::function<void(const std::string& sdp)> onLocalAnswer;
  std::function<void(const std::string& candidate, const std::string& mid, int index)> onLocalIce;

 private:
  webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory_;
  webrtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_;
  webrtc::scoped_refptr<PConnectionObserver> observer_;
};

#endif