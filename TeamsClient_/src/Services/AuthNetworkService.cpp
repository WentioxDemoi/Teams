#include "AuthNetworkService.h"

AuthNetworkService::AuthNetworkService(QObject* parent)
    : QObject(parent)
{
    connect(&socket_, &QSslSocket::readyRead, this, [this]() {
        
        QByteArray data = socket_.readAll();
        

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            emit authError("Malformed JSON response from server");
            return;
        }

        QJsonObject root = doc.object();

        if (root.contains("error") && root["error"].isString()) {
            emit authError(root["error"].toString());
            return;
        }

        if (!root.contains("data") || !root["data"].isObject()) {
            emit authError("Missing data field in server response");
            return;
        }

        User user = User::fromJson(root["data"].toObject());
        if (!user.isValid()) {
            emit authError("Invalid user data received");
            return;
        }

        qDebug() << "AUTH SUCCESS";
        qDebug() << "ID:" << user.id();
        qDebug() << "TOKEN:" << user.token();
        qDebug() << "Username:" << user.username();
        qDebug() << "Status:" << user.status();

        emit authSuccess(user);
    });

    connect(&socket_, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors),
        this, [this](const QList<QSslError> &errors) {
    for (const auto &err : errors)
        qDebug() << "SSL error:" << err.errorString();
    socket_.ignoreSslErrors(errors);
});
}

void AuthNetworkService::login(const QString& username, const QString& password)
{
    QJsonObject payload{
        {"type", "login"},
        {"username", username},
        {"password", password}
    };
    sendRequest(payload);
}

void AuthNetworkService::registerUser(const QString& username, const QString& password)
{
    QJsonObject payload{
        {"type", "register"},
        {"username", username},
        {"password", password}
    };
    sendRequest(payload);
}

void AuthNetworkService::sendRequest(const QJsonObject& payload)
{
    if (!socket_.isOpen()) {
        socket_.connectToHostEncrypted("localhost", 8080);
        if (!socket_.waitForEncrypted(3000)) {
            emit authError("Failed to connect to server: " + socket_.errorString());
            return;
        }
    }

    QByteArray message = QJsonDocument(payload).toJson(QJsonDocument::Compact) + "\n";
    socket_.write(message);
    socket_.flush();
}