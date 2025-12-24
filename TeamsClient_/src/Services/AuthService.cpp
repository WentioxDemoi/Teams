#include "AuthService.h"
#include "../Services/NetworkService.h"

#include <QJsonDocument>
#include <QJsonObject>

AuthService::AuthService(NetworkService* network, QObject* parent)
    : QObject(parent),
      network_(network)
{
    connect(network_, &NetworkService::reponseReceived,
            this, &AuthService::handleAuthResponse);

    connect(network_, &NetworkService::errorOccurred,
            this, &AuthService::handleNetworkError);
}

void AuthService::loginUser(const QString& username,
                            const QString& password)
{
    QJsonObject payload{
        {"type", "login"},
        {"username", username},
        {"password", password}
    };

    network_->sendRaw(
        QJsonDocument(payload).toJson(QJsonDocument::Compact)
    );
}

void AuthService::registerUser(const QString& username,
                               const QString& password)
{
    QJsonObject payload{
        {"type", "register"},
        {"username", username},
        {"password", password}
    };

    network_->sendRaw(
        QJsonDocument(payload).toJson(QJsonDocument::Compact)
    );
}

void AuthService::handleAuthResponse(const QByteArray& data)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        emit authError("Invalid server response");
        return;
    }

    QJsonObject root = doc.object();

    if (root.contains("error")) {
        emit authError(root["error"].toString());
        return;
    }

    if (!root.contains("data") || !root["data"].isObject()) {
        emit authError("Missing data field");
        return;
    }

    User user = User::fromJson(root["data"].toObject());

    if (!user.isValid()) {
        emit authError("Invalid user data");
        return;
    }

    emit authSuccess(user);
}

void AuthService::handleNetworkError(const QString& error)
{
    emit authError(error);
}