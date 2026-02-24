#ifndef WEBRTCSERVICE_H
#define WEBRTCSERVICE_H

#include "../webrtc_includes.h" 
#include "../includes.h"

class WebRTCService : public QObject,
                      public webrtc::PeerConnectionObserver,
                      public webrtc::CreateSessionDescriptionObserver {
  Q_OBJECT

public:
  explicit WebRTCService(QObject *parent = nullptr);

  // --- lifecycle ---
  void connectToSignaling();
  void disconnectFromSignaling();

  void initialize(); // init PeerConnection + tracks
  void startCall();  // user action → create OFFER


  // --- local video ---
  void pushVideoFrame(const webrtc::VideoFrame &frame);

signals:
  // UI / ViewModel
  void connected();
  void disconnected();

  void remoteVideoFrame(const webrtc::VideoFrame &frame);
  void errorOccurred(const QString &message);

protected:
  // --- signaling handlers ---
  void handleServerMessage(const QByteArray &data);

  void sendJson(const QJsonObject &obj) { return; };

  void onOffer(const QString &sdp);
  void onAnswer(const QString &sdp);
  void onIce(const QString &candidate, const QString &mid, int index);

  // --- WebRTC callbacks ---
  // PeerConnectionObserver
  void OnIceCandidate(const webrtc::IceCandidateInterface *candidate) override;

  // void OnTrack(
  //     webrtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) override;

  // CreateSessionDescriptionObserver
  void OnSuccess(webrtc::SessionDescriptionInterface *desc) override;

  // void OnFailure(webrtc::RTCError error) override;

private:
  // --- signaling ---
  QSslSocket socket_;
  QByteArray buffer_;

  // --- WebRTC ---
  webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory_;
  webrtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_;
  webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> videoSource_;

  void initPeerConnection();
};

class RemoteDescObserver : public webrtc::SetRemoteDescriptionObserverInterface {
public:
    static webrtc::scoped_refptr<RemoteDescObserver> Create(
        std::function<void(webrtc::RTCError)> cb) {
        return webrtc::make_ref_counted<RemoteDescObserver>(std::move(cb));
    }
    explicit RemoteDescObserver(std::function<void(webrtc::RTCError)> cb)
        : cb_(std::move(cb)) {}
    void OnSetRemoteDescriptionComplete(webrtc::RTCError error) override {
        cb_(std::move(error));
    }
private:
    std::function<void(webrtc::RTCError)> cb_;
};

// Helper: wrap a lambda into a SetLocalDescriptionObserverInterface
class LocalDescObserver : public webrtc::SetLocalDescriptionObserverInterface {
public:
    static webrtc::scoped_refptr<LocalDescObserver> Create(
        std::function<void(webrtc::RTCError)> cb) {
        return webrtc::make_ref_counted<LocalDescObserver>(std::move(cb));
    }
    explicit LocalDescObserver(std::function<void(webrtc::RTCError)> cb)
        : cb_(std::move(cb)) {}
    void OnSetLocalDescriptionComplete(webrtc::RTCError error) override {
        cb_(std::move(error));
    }
private:
    std::function<void(webrtc::RTCError)> cb_;
};

#endif // WEBRTCSERVICE_H


