#include "AuthNetworkService.h"

AuthNetworkService::AuthNetworkService(QObject *parent) : QObject(parent) {
  waitingForResponse_ = false;

  connect(&socket_, &QSslSocket::encrypted, this,
          [this]() { sendPendingPayload(); });

  connect(&socket_, &QSslSocket::readyRead, this,
          [this]() { handleServerResponse(socket_.readAll()); });

  connect(&socket_,
          QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors), this,
          [this](const QList<QSslError> &errors) {
            for (const auto &err : errors)
              qDebug() << "SSL error:" << err.errorString();
            socket_.ignoreSslErrors(errors);
          });
}

void AuthNetworkService::loginUser(const QString &email,
                                   const QString &password) {
  QJsonObject payload{
      {"type", "login"}, {"email", email}, {"password", password}};
  sendRequest(payload);
}

void AuthNetworkService::registerUser(const QString &email,
                                      const QString &username,
                                      const QString &password) {
  QJsonObject payload{{"type", "register"},
                      {"email", email},
                      {"username", username},
                      {"password", password}};
  sendRequest(payload);
}

void AuthNetworkService::validateToken(const QString &key,
                                       const QString &value) {
  if (key.compare("token")) {
    qDebug() << "TOKEN";
    QJsonObject payload{{"type", "validate_token"}, {"token", value}};
    sendRequest(payload);
  }
}

void AuthNetworkService::sendRequest(const QJsonObject &payload) {
  if (waitingForResponse_)
    return;
  waitingForResponse_ = true;
  pendingPayload_ = payload;
  if (socket_.state() == QAbstractSocket::ConnectedState &&
      socket_.isEncrypted()) {
    sendPendingPayload();
  } else {
    socket_.connectToHostEncrypted("localhost", 8080);
  }
}

void AuthNetworkService::sendPendingPayload() {
  QByteArray message =
      QJsonDocument(pendingPayload_).toJson(QJsonDocument::Compact) + "\n";

  socket_.write(message);
}

void AuthNetworkService::handleServerResponse(const QByteArray &data) {
  waitingForResponse_ = false;

  QJsonParseError parseError;
  QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

  if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
    emit authError("Malformed JSON response from server");
    return;
  }

  QJsonObject root = doc.object();

  if (root.contains("error") && root["error"].isString()) {
    if (root["error"].toString().contains("Login failed: invalid token.")) {
      emit invalidToken("Token is not valid");
      return;
    }

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
  qDebug() << "Letsogooooo";
  emit authSuccess(user);
}