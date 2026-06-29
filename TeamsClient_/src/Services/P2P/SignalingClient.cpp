#include "SignalingClient.h"

#include "../includes.h"
#include "TokenManager.h"

SignalingClient::SignalingClient(QObject* parent)
    : QObject(parent),
      network_(new NetworkService(8081, this))  // UUID de e@gmail.com e
{
  connect(network_, &NetworkService::jsonReceived, this, &SignalingClient::handleServerResponse);

  connect(network_, &NetworkService::networkError, this,
          [](const QString& err) { qWarning() << "Network error in SignalingClient:" << err; });

  qDebug() << "SignalingClient initialized with UUID:" << clientId_;
}

void SignalingClient::sendOffer(const QString& sdp) {
  qDebug() << "[SignalingClient] sendOffer called, sdp size=" << sdp.size();
  sendMessage("offer", sdp);
}

void SignalingClient::sendAnswer(const QString& sdp) { sendMessage("answer", sdp); }

void SignalingClient::sendIce(const QString& payload) { sendMessage("ice", payload); }



void SignalingClient::sendMessage(const QString& type, const QString& payload) {
  QJsonObject message;
  message["type"] = type;
  message["target_uuid"] = QString::fromStdString(uuidd);

  if (type == "ice") {
    QJsonDocument doc = QJsonDocument::fromJson(payload.toUtf8());
    if (doc.isObject()) {
      QJsonObject obj = doc.object();
      message["candidate"] = obj.value("candidate");
      message["mid"] = obj.value("mid");
      message["index"] = obj.value("index");
    } else {
      qWarning() << "ICE payload invalid JSON:" << payload;
      return;
    }
  } else {
    message["sdp"] = payload;
  }

  network_->send(message);
}

void SignalingClient::registerWithServer4WebRTC(QString UUID) {
  Q_UNUSED(UUID);
  Q_UNUSED(UUID);
  const QString token = TokenManager::instance().token;

  if (token.isEmpty()) {
    qWarning() << "[SignalingClient] Cannot register: token is empty";
    return;
  }

  qDebug() << "[SignalingClient] Sending register with token";

  QJsonObject message;
  message["type"] = "register";
  message["token"] = token;

  network_->send(message);
}

void SignalingClient::disconnectFromServer() { network_->disconnectFromServer(); }