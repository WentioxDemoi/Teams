#ifndef WEBRTCSERVICE_H
#define WEBRTCSERVICE_H

#include <QObject>

#include "PConnectionController.h"
#include "SignalingClient.h"

/**
 * @class WebRTCService
 * @brief Service de gestion des communications WebRTC.
 *
 * Coordonne la connexion avec le serveur de signalisation et le contrôle des sessions P2P,
 * en gérant les appels entrants et sortants, les flux vidéo distants, et les événements d'état
 * ou d'erreur liés à la communication.
 */
class WebRTCService : public QObject {
  Q_OBJECT
 public:
  explicit WebRTCService(QObject* parent = nullptr);
  void disconnectFromSignalingServer();

 public slots:
  void startCall();
  void acceptCall();
  void hangup();

 private slots:
  void onRemoteOffer(QString sdp);
  void onRemoteAnswer(QString sdp);
  void onRemoteIce(QString candidate, QString mid, int index);

 signals:
  void connected();
  void disconnected();
  void remoteVideoFrame(const webrtc::VideoFrame&);
  void error(QString);
  void onP2PChange(bool inProgress);
  void registerWithServer4WebRTC(QString UUID);

 private:
  std::unique_ptr<PConnectionController> pConnectionController_;
  SignalingClient* signalingClient_;
};
#endif
