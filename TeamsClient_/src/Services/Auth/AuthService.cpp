#include "AuthService.h"

#include "../../Core/ServiceLocator.h"
#include "../../Utils/TokenManager.h"
#include <cstdlib>

AuthService::AuthService(NetworkService* network, IUserService* userService, ITokenManager* token,
                         QObject* parent)
    : IAuthService(parent),
      network_(network ? network : new NetworkService(8080, parent)),
      userService_(userService ? userService
                               : ServiceLocator::instance().getService<IUserService>()),
      token_(token ? token : &TokenManager::instance()) {
  Q_ASSERT(network_);
  Q_ASSERT(userService_);
  Q_ASSERT(token_);

  connect(network_, &NetworkService::jsonReceived, this, &AuthService::handleServerResponse);

  connect(network_, &NetworkService::networkError, this, &AuthService::authError);

  connect(this, &IAuthService::authSuccess, userService_, &IUserService::saveUser);
  connect(userService_, &IUserService::userSaved, this, &IAuthService::onUserSaved);
}

void AuthService::start() {
  if (/*!*/token_->token.isEmpty()) {
    // network_->send({{"type", "validate_token"}, {"token", token_->token}});
    network_->send({{"type", "validate_token"}, {"token", "313729ff01aed14d0e32fd009ac39dc8"}});
  }
  else
    errorToken("No token found");
}

void AuthService::loginUser(const QString& email, const QString& password) {
  network_->send({{"type", "login"}, {"email", email}, {"password", password}});
}

void AuthService::registerUser(const QString& email, const QString& username,
                               const QString& password) {
  network_->send({{"type", "register"},
                  {"email", email},
                  {"username", username},
                  {"password", password}});
}

void AuthService::onUserSaved(const User& user) {
  User localUser = user;

  if (!user.token().isEmpty()) {
    token_->writeToken(localUser.token());
    localUser.clearToken();
    emit authSuccess(localUser);
  } else {
    qDebug() << "Pas de token dans le user (Ligne 46 AuthService)";
  }
}

void AuthService::errorToken(const QString& error) {
  qDebug() << error;
  if (error.contains("Token is not valid")) token_->deleteToken();

  userService_->deleteAll();
  emit noTokenFound();
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
      errorToken("Token is not valid");
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

    userService_->saveUser(user);
    return;
  }
}

void AuthService::disconnectFromServer() {
    network_->disconnectFromServer();
}