#include "SignalingClient.h"
#include <QJsonDocument>
#include <QDebug>

SignalingClient::SignalingClient(QObject *parent)
    : QObject(parent),
      network_(new NetworkService(8081, this)),
      clientId_("8295ac0c-142d-43e1-9554-d4d271a7686a") // UUID de e@gmail.com e
      {

    connect(network_, &NetworkService::jsonReceived, this,
            &SignalingClient::handleServerResponse);

    connect(network_, &NetworkService::networkError, this, [](const QString &err) {
        qWarning() << "Network error in SignalingClient:" << err;
    });

    qDebug() << "SignalingClient initialized with UUID:" << clientId_;
}

void SignalingClient::sendOffer(const QString &sdp) {
    sendMessage("offer", sdp);
}

void SignalingClient::sendAnswer(const QString &sdp) {
    sendMessage("answer", sdp);
}

void SignalingClient::sendIce(const QString &payload) {
    sendMessage("ice", payload);
}

void SignalingClient::handleServerResponse(const QJsonObject &root) {
    if (!root.contains("type") || !root["type"].isString())
        return;

    const QString type = root["type"].toString();

    if (type == "offer" && root.contains("sdp") && root["sdp"].isString()) {
        emit offerReceived(root["sdp"].toString());
    } else if (type == "answer" && root.contains("sdp") && root["sdp"].isString()) {
        emit answerReceived(root["sdp"].toString());
    } else if (type == "ice" && root.contains("candidate") && root.contains("mid") && root.contains("index")) {
        emit iceReceived(root["candidate"].toString(),
                         root["mid"].toString(),
                         root["index"].toInt());
    }
}

void SignalingClient::sendMessage(const QString &type, const QString &payload) {
    QJsonObject message;
    message["type"] = type;
    message["uuid"] = clientId_;

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
    qDebug() << "Sent register ping for UUID:" << clientId_;
    QJsonObject message;
    message["type"] = "register";
    message["uuid"] = UUID;
    network_->send(message);
}