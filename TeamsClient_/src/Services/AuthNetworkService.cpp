#include "AuthNetworkService.h"

AuthNetworkService::AuthNetworkService(QObject *parent) : QObject(parent) {
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
  qDebug() << "\n=== handleServerResponse START ===";
  qDebug() << "Raw data received:" << data;

  if (data.trimmed().isEmpty()) {
    qDebug() << "Data is empty, returning early";
    return;
  }

  QJsonParseError parseError;
  QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

  if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
    qDebug() << "JSON parse error:" << parseError.errorString();
    emit authError("Malformed JSON response from server");
    return;
  }

  waitingForResponse_ = false;

  QJsonObject root = doc.object();
  qDebug() << "Parsed JSON object:" << root;

  // Vérification du type
  if (!root.contains("type") || !root["type"].isString()) {
    qDebug() << "Missing or invalid 'type' field in JSON";
    emit authError("Missing or invalid type field in server response");
    return;
  }

  const QString type = root["type"].toString();
  qDebug() << "Response type:" << type;

  // Gestion des erreurs
  if (root.contains("error") && root["error"].isString()) {
    const QString error = root["error"].toString();
    qDebug() << "Server returned error:" << error;

    if (type == "validate_token_response" &&
        error.contains("invalid token", Qt::CaseInsensitive)) {
      qDebug() << "Token invalid detected";
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
      qDebug() << "Missing or invalid 'data' field in JSON";
      emit authError("Missing data field in server response");
      return;
    }

    QJsonObject userJson = root["data"].toObject();
    qDebug() << "User JSON object:" << userJson;

    User user = User::fromJson(userJson);
    qDebug() << "User object created:";
    user.print();

    if (!user.isValid()) {
      qDebug() << "User is not valid according to isValid()";
      emit authError("Invalid user data received");
      return;
    }

    qDebug() << "User is valid, emitting authSuccess";
    emit authSuccess(user);
    return;
  }

  // Au cas où requête inconnue
  qDebug() << "Unhandled response type:" << type;
  emit authError(QString("Unhandled response type: %1").arg(type));

  qDebug() << "=== handleServerResponse END ===\n";
}