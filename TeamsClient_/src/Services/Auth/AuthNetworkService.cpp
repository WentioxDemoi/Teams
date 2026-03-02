#include "AuthNetworkService.h"

AuthNetworkService::AuthNetworkService(NetworkService *network, QObject *parent)
    : network_(network ? network : new NetworkService(8080, parent)),
      IAuthNetworkService(parent) {

  connect(network_, &NetworkService::jsonReceived, this,
          &AuthNetworkService::handleServerResponse);

  connect(network_, &NetworkService::networkError, this,
          &AuthNetworkService::authError);
}

void AuthNetworkService::loginUser(const QString &email,
                                   const QString &password) {
  network_->send({{"type", "login"}, {"email", email}, {"password", password}});
}

void AuthNetworkService::registerUser(const QString &email,
                                      const QString &username,
                                      const QString &password) {
  network_->send({{"type", "register"},
                  {"email", email},
                  {"username", username},
                  {"password", password}});
}

void AuthNetworkService::validateToken(const QString &value) {
  network_->send({{"type", "validate_token"}, {"token", value}});
}

void AuthNetworkService::handleServerResponse(const QJsonObject &root) {

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
    emit registerWithServer4WebRTC(user.uuid());
    return;
  }
}

void AuthNetworkService::disconnectFromServer() {
    network_->disconnectFromServer();
}