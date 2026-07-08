#ifndef WEBRTCSERVICE_H
#define WEBRTCSERVICE_H

#include <QObject>
#include <functional>

#include "PConnectionController.h"


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
  explicit WebRTCService(QObject *parent);

  void disconnectFromSignalingServer();

void setCallBacks(std::function<void(const std::string &sdp)> onLocalOffer,
                   std::function<void(const std::string &sdp)> onLocalAnswer,
                   std::function<void(const std::string &candidate, const std::string &mid, int index)> onLocalIce,
                   std::function<void(bool isConnected)> isContactConnectedChanged);
  void startCall();
  void acceptCall();
  void hangup();

  void onRemoteOffer(QString sdp);
  void onRemoteAnswer(QString sdp);
  void onRemoteIce(QString candidate, QString mid, int index);

  void setMicEnabled(bool enabled) {
    if (pConnectionController_)
      pConnectionController_->setMicEnabled(enabled);
  }

signals:
  void connected();
  void disconnected();
  void remoteVideoFrame(const webrtc::VideoFrame &);
  void error(QString);

private:
  std::unique_ptr<PConnectionController> pConnectionController_;
};
#endif
