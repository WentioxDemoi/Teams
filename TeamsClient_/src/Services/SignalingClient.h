#ifndef SIGNALINGCLIENT_H
#define SIGNALINGCLIENT_H

#include "../includes.h"
#include "NetworkService.h"
#include <QtCore/qjsonobject.h>
#include <QtCore/qobject.h>
#include <QUuid>

class SignalingClient : public QObject {
    Q_OBJECT
public:
    explicit SignalingClient(QObject *parent = nullptr);

    void sendOffer(const QString &sdp);
    void sendAnswer(const QString &sdp);
    void sendIce(const QString &payload);


signals:
    void offerReceived(QString sdp);
    void answerReceived(QString sdp);
    void iceReceived(QString candidate, QString mid, int index);

private slots:
    void handleServerResponse(const QJsonObject &root);

public slots:
    void registerWithServer4WebRTC(QString UUID);


private:
    NetworkService *network_;
    QString clientId_;

    void sendMessage(const QString &type, const QString &payload);
};

#endif