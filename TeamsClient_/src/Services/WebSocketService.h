#ifndef WEBSOCKETSERVICE_H
#define WEBSOCKETSERVICE_H

#include "../../includes.h"

class WebSocketService : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketService(QObject* parent = nullptr);

    void connectToServer(const QUrl& url);
    void sendMessage(const QByteArray& message);

signals:
    void messageReceived(const QByteArray& message);
    void errorOccurred(const QString& error);

private:
    QWebSocket socket_;
};

#endif