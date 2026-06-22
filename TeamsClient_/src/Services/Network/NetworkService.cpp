#include "NetworkService.h"

#include "../../Core/State/UserState.h"
#include "SessionEnum.h"

NetworkService::NetworkService(qint16 port, QObject* parent) : QObject(parent), port_(port) {
  switch (port) {
    case 8080:
      server_ = ServerType::Auth;
      break;
    case 8081:
      server_ = ServerType::WebRTC;
      break;
    case 8082:
      server_ = ServerType::Message;
      break;
    case 8083:
      server_ = ServerType::Call;
      break;
    case 8084:
      server_ = ServerType::Contact;
      break;
    default:
      qDebug() << "Unknown enum (NetworkService)";
  }

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
    emit connectionUpdate(server_, false);
    qDebug() << "Disconnected from server with port" << port_;
  });

  connect(&socket_, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors), this,
          [this](const QList<QSslError>& errors) {
            for (const auto& err : errors) qWarning() << "SSL error:" << err.errorString();
            socket_.ignoreSslErrors(errors);
          });

  connect(&socket_, &QSslSocket::encrypted, this, [this]() {
    qDebug() << "SSL ready -> flushing queue";
    emit connectionUpdate(server_, true);
    while (!pendingQueue_.isEmpty()) send(pendingQueue_.dequeue());
  });

  connect(&UserState::instance(), &UserState::localUserSaved, this, &NetworkService::auth);

  ensureConnected();
}

void NetworkService::ensureConnected() {
  if (socket_.state() == QAbstractSocket::UnconnectedState) {
    socket_.connectToHostEncrypted("localhost", port_);
  }
}

void NetworkService::send(const QJsonObject& payload) {
  qDebug() << "[NetworkService] send called, encrypted=" << socket_.isEncrypted() << payload;
  if (payload.isEmpty()) {
    qWarning() << "Refusing to send empty payload";
    return;
  }

  ensureConnected();

  if (!socket_.isEncrypted()) {
    qWarning() << "Socket not encrypted yet, send deferred";
    pendingQueue_.enqueue(payload);
    return;
  }

  QByteArray message = QJsonDocument(payload).toJson(QJsonDocument::Compact) + "\n";

  socket_.write(message);
}

void NetworkService::handleIncomingData(const QByteArray& data) {
  if (data.isEmpty()) return;

  QJsonParseError parseError;
  const QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

  if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
    emit networkError("Malformed JSON received from server");
    return;
  }

  const QJsonObject root = doc.object();

  if (!root.contains("type") || !root["type"].isString()) {
    emit networkError("Missing or invalid type field in server response");
    return;
  }

  emit jsonReceived(root);
}

void NetworkService::auth(const User &user) {
  QJsonObject payload;
  payload["token"] = user.token();
  send(payload);
}

void NetworkService::disconnectFromServer() {
  if (socket_.state() != QAbstractSocket::UnconnectedState) {
    socket_.disconnectFromHost();
    socket_.waitForDisconnected(3000);
  }
  buffer_.clear();
}