#include "AuthService.h"

#include <cstdlib>

#include "../../Core/ServiceLocator.h"

AuthService::AuthService(NetworkService* network, QObject* parent)
    : IAuthService(parent), network_(network ? network : new NetworkService(8080, parent)) {
  Q_ASSERT(network_);

  connect(network_, &NetworkService::jsonReceived, this, &AuthService::handleServerResponse);

  connect(network_, &NetworkService::networkError, this, &AuthService::authError);

  // connect(this, &IAuthService::authSuccess, userService_, &IUserService::saveUser);
  // connect(userService_, &IUserService::userSaved, this, &IAuthService::onUserSaved);
}

void AuthService::loginUser(const QString& email, const QString& password) {
  network_->send({{"type", "login"}, {"email", email}, {"password", password}});
}

void AuthService::registerUser(const QString& firstName, const QString& lastName,
                               const QString& email, const QString& password) {
  network_->send({{"type", "register"},
                  {"firstName", firstName},
                  {"lastName", lastName},
                  {"email", email},
                  {"password", password}});
}

void AuthService::loginWithToken(const QString &token)
{
  network_->send({{"type", "validate_token"}, {"token", token}});
}

void AuthService::handleServerResponse(const QJsonObject& root) {
  if (!root.contains("type") || !root["type"].isString()) {
    emit authError("Missing or invalid type field in server response");
    return;
  }

  const QString type = root["type"].toString();
  qDebug() << "Response type:" << type;

  if (root.contains("error") && root["error"].isString()) {
    const QString error = root["error"].toString();

    if (type == "validate_token_response" && error.contains("invalid token", Qt::CaseInsensitive)) {
      emit errorToken("Token is not valid");
      return;
    }

    emit authError(error);
    return;
  }

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
}

void AuthService::disconnectFromServer() { network_->disconnectFromServer(); }