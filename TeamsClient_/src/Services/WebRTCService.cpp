#include "WebRTCService.h"

WebRTCService::WebRTCService(QObject *parent) : QObject(parent) {
  connect(&socket_, &QSslSocket::encrypted, this,
          [this]() { qDebug() << "WebRTC signaling connected"; });

  connect(&socket_, &QSslSocket::readyRead, this, [this]() {
    buffer_ += socket_.readAll();
    while (buffer_.contains('\n')) {
      QByteArray msg = buffer_.left(buffer_.indexOf('\n'));
      buffer_.remove(0, buffer_.indexOf('\n') + 1);
      handleServerMessage(msg);
    }
  });

  connect(&socket_, &QSslSocket::sslErrors, this,
          [this](const QList<QSslError> &errors) {
            socket_.ignoreSslErrors(errors);
          });
connectToSignaling();
}

void WebRTCService::connectToSignaling() {
  if (socket_.state() == QAbstractSocket::UnconnectedState)
    socket_.connectToHostEncrypted("localhost", 8081);
}

void WebRTCService::initialize() {
  factory_ = webrtc::CreatePeerConnectionFactory(
      nullptr, nullptr, nullptr, 
      nullptr,
      webrtc::CreateBuiltinAudioEncoderFactory(),
      webrtc::CreateBuiltinAudioDecoderFactory(),
      webrtc::CreateBuiltinVideoEncoderFactory(),
      webrtc::CreateBuiltinVideoDecoderFactory(),
       nullptr, nullptr);

//   videoSource_ = webrtc::make_ref_counted<QtVideoSource>();
//   initPeerConnection();

//   auto track = factory_->CreateVideoTrack("video", videoSource_);
//   peer_->AddTrack(track, {"stream"});
}

void WebRTCService::startCall() {
  qDebug() << "User initiates call";
//   peer_->CreateOffer(this, {});
}

void WebRTCService::onOffer(const QString &sdp) {
    // webrtc::SdpParseError err;
    // auto desc = webrtc::CreateSessionDescription(
    //     webrtc::SdpType::kOffer, sdp.toStdString(), &err);
    // peer_->SetRemoteDescription(
    //     std::move(desc),
    //     RemoteDescObserver::Create([](webrtc::RTCError e) {
    //         if (!e.ok()) qDebug() << "SetRemoteDescription error:" << e.message();
    //     }));
    // peer_->CreateAnswer(this, {});
}

void WebRTCService::onAnswer(const QString &sdp) {
    // webrtc::SdpParseError err;
    // auto desc = webrtc::CreateSessionDescription(
    //     webrtc::SdpType::kAnswer, sdp.toStdString(), &err);
    // peer_->SetRemoteDescription(
    //     std::move(desc),
    //     RemoteDescObserver::Create([](webrtc::RTCError e) {
    //         if (!e.ok()) qDebug() << "SetRemoteDescription error:" << e.message();
    //     }));
}

void WebRTCService::OnSuccess(webrtc::SessionDescriptionInterface* desc) {
    // Capture before moving
    std::string sdp, type;
    desc->ToString(&sdp);
    type = desc->type();

    peer_->SetLocalDescription(
        absl::WrapUnique(desc),
        LocalDescObserver::Create([](webrtc::RTCError e) {
            if (!e.ok()) qDebug() << "SetLocalDescription error:" << e.message();
        }));

    QJsonObject msg;
    msg["type"] = QString::fromStdString(type);
    msg["sdp"]  = QString::fromStdString(sdp);
    sendJson(msg);
}

void WebRTCService::onIce(const QString &candidate, const QString &mid,
                          int index) {
//   webrtc::SdpParseError err;
//   auto ice = webrtc::CreateIceCandidate(mid.toStdString(), index,
//                                         candidate.toStdString(), &err);

//   peer_->AddIceCandidate(ice);
}

void WebRTCService::OnIceCandidate(
    const webrtc::IceCandidateInterface* candidate)
{
    std::string sdp;
    candidate->ToString(&sdp);

    QJsonObject msg;
    msg["type"] = "ice";
    msg["candidate"] = QString::fromStdString(sdp);
    msg["sdpMid"] = QString::fromStdString(candidate->sdp_mid());
    msg["sdpMLineIndex"] = candidate->sdp_mline_index();

    sendJson(msg);
}

void WebRTCService::handleServerMessage(const QByteArray &data) {
  QJsonDocument doc = QJsonDocument::fromJson(data);
  if (!doc.isObject())
    return;

  QJsonObject msg = doc.object();
  QString type = msg["type"].toString();

  if (type == "offer")
    onOffer(msg["sdp"].toString());
  else if (type == "answer")
    onAnswer(msg["sdp"].toString());
  else if (type == "ice")
    onIce(msg["candidate"].toString(), msg["sdpMid"].toString(),
          msg["sdpMLineIndex"].toInt());
}