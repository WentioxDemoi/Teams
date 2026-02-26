#include "NetworkService.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

NetworkService::NetworkService(qint16 port, QObject *parent)
    : QObject(parent), port_(port) {

    connect(&socket_, &QSslSocket::readyRead, this, [this]() {
        buffer_ += socket_.readAll();
        while (buffer_.contains('\n')) {
            const int idx = buffer_.indexOf('\n');
            QByteArray msg = buffer_.left(idx);
            buffer_.remove(0, idx + 1);
            handleIncomingData(msg);
        }
    });

    connect(&socket_, &QSslSocket::disconnected, this, [this]() {
        buffer_.clear();
        qDebug() << "Disconnected from server with port" << port_;
    });

    connect(&socket_,
            QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
            this,
            [this](const QList<QSslError> &errors) {
                for (const auto &err : errors)
                    qWarning() << "SSL error:" << err.errorString();
                socket_.ignoreSslErrors(errors);
            });

    ensureConnected();
}

void NetworkService::ensureConnected() {
    if (socket_.state() == QAbstractSocket::UnconnectedState) {
        socket_.connectToHostEncrypted("localhost", port_);
    }
}

void NetworkService::send(const QJsonObject &payload) {
    if (payload.isEmpty()) {
        qWarning() << "Refusing to send empty payload";
        return;
    }

    ensureConnected();

    if (!socket_.isEncrypted()) {
        qWarning() << "Socket not encrypted yet, send deferred";
        return;
    }

    QByteArray message =
        QJsonDocument(payload).toJson(QJsonDocument::Compact) + "\n";

    socket_.write(message);
}

void NetworkService::handleIncomingData(const QByteArray &data) {
    if (data.trimmed().isEmpty())
        return;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        emit networkError("Malformed JSON received from server");
        return;
    }

    emit jsonReceived(doc.object());
}

void NetworkService::disconnectFromServer() {
    if (socket_.state() != QAbstractSocket::UnconnectedState) {
        socket_.disconnectFromHost();
        socket_.waitForDisconnected(3000);
    }
    qDebug() << "Disconnected from server with port" << port_;
    buffer_.clear();
}