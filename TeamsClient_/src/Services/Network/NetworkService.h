#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include <QObject>
#include <QSslSocket>
#include <QAbstractSocket>
#include <QByteArray>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QList>
#include <QSslError>

class NetworkService : public QObject {
    Q_OBJECT

public:
    explicit NetworkService(qint16 port, QObject *parent = nullptr);

    void send(const QJsonObject &payload);
    void disconnectFromServer();
    

signals:
    void jsonReceived(const QJsonObject &json);
    void networkError(const QString &error);

private:
    void handleIncomingData(const QByteArray &data);
    void ensureConnected();

private:
    QSslSocket socket_;
    QByteArray buffer_;
    qint16 port_;
};

#endif