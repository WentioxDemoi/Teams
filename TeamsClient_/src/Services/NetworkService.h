#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include <QObject>
#include <QSslSocket>
#include <QJsonObject>
#include <QtCore/qtypes.h>

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