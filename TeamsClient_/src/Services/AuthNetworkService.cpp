#include "AuthNetworkService.h"

AuthNetworkService::AuthNetworkService(QObject *parent) : IAuthNetworkService(parent) {
  waitingForResponse_ = false;

  connect(&socket_, &QSslSocket::encrypted, this,
          [this]() { sendPendingPayload(); });

  connect(&socket_, &QSslSocket::readyRead, this, [this]() {
    buffer_ += socket_.readAll();
    while (buffer_.contains('\n')) {
      auto msg = buffer_.left(buffer_.indexOf('\n'));
      buffer_.remove(0, buffer_.indexOf('\n') + 1);
      handleServerResponse(msg);
    }
  });

  connect(&socket_, &QSslSocket::disconnected, this, [this]() {
    waitingForResponse_ = false;
    buffer_.clear();
  });

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

void AuthNetworkService::validateToken(const QString &value) {
  QJsonObject payload{{"type", "validate_token"}, {"token", value}};
  sendRequest(payload);
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
  if (data.trimmed().isEmpty()) {
    return;
  }

  QJsonParseError parseError;
  QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

  if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
    emit authError("Malformed JSON response from server");
    return;
  }

  waitingForResponse_ = false;

  QJsonObject root = doc.object();

  if (!root.contains("type") || !root["type"].isString()) {
    emit authError("Missing or invalid type field in server response");
    return;
  }

  const QString type = root["type"].toString();
  qDebug() << "Response type:" << type;

  if (root.contains("error") && root["error"].isString()) {
    const QString error = root["error"].toString();

    if (type == "validate_token_response" &&
        error.contains("invalid token", Qt::CaseInsensitive)) {
      emit invalidToken("Token is not valid");
      return;
    }

    emit authError(error);
    return;
  }

  // Retours avec comme objet un User
  if (type == "login_response" || type == "register_response" ||
      type == "validate_token_response") {

    if (!root.contains("data") || !root["data"].isObject()) {
      emit authError("Missing data field in server response");
      return;
    }

    QJsonObject userJson = root["data"].toObject();

    User user = User::fromJson(userJson);
    user.setIsMe(true);
    user.print();

    if (!user.isValid()) {
      qDebug() << "User pas valide selon user.isValid";
      return;
    }

    emit authSuccess(user);
    return;
  }

  emit authError(QString("Unhandled response type: %1").arg(type));
}

void AuthNetworkService::disconnectFromServer() {
    if (socket_.state() == QAbstractSocket::ConnectedState ||
        socket_.state() == QAbstractSocket::ConnectingState) {
        socket_.disconnectFromHost();
        if (socket_.state() != QAbstractSocket::UnconnectedState) {
            socket_.waitForDisconnected(3000); // optionnel : attendre max 3s
        }
    }

    // Réinitialisation de l’état
    waitingForResponse_ = false;
    buffer_.clear();
    pendingPayload_ = QJsonObject();
    
    qDebug() << "Disconnected from server";
}