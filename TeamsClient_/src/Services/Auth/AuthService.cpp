#include "AuthService.h"
#include "ServiceLocator.h"

#include <cstdlib>
#include "TokenManager.h"

#include "Network/NetworkService.h"

AuthService::AuthService(NetworkService* network, IUserService* userService,
                         ITokenManager* tokenManager, QObject* parent)
    : IAuthService(parent),
      network_(network ? network : new NetworkService(8080, parent)),
      userService_(userService ? userService : ServiceLocator::instance().getService<IUserService>()),
      tokenManager_(tokenManager ? tokenManager : &TokenManager::instance()) {
  Q_ASSERT(network_);
  Q_ASSERT(userService_);
  Q_ASSERT(tokenManager_);

  connect(network_, &NetworkService::jsonReceived, this, &AuthService::handleServerResponse);
  connect(network_, &NetworkService::networkError, this, &AuthService::authError);
  connect(network_, &NetworkService::connectionUpdate, this, &AuthService::connectionUpdate);
  connect(this, &IAuthService::errorToken, this, &AuthService::handleTokenError);
  connect(userService_, &IUserService::localUserSaved, this, &AuthService::handleLocalUserSaved);
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

void AuthService::loginWithToken(void)
{
  if (/*!*/tokenManager_->token.isEmpty()) {
    //network_->send({{"type", "validate_token"}, {"token", tokenManager_->token}});
    network_->send({{"type", "validate_token"}, {"token", "f62e4f0ec56cfa17c7361f933aaccf0b"}});
  } else {
    emit noTokenFound();
  }
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

    if (!user.token().isEmpty()) {
      tokenManager_->writeToken(user.token());
      userService_->saveUser(user);
    } else {
      emit authError("No token received from server");
    }

    userService_->saveUser(user);
    return;
  }
}

void AuthService::handleLocalUserSaved(const User& user) {
  User localUser = user;
  localUser.clearToken();
  emit authSuccess(localUser);
}

void AuthService::handleTokenError(const QString& error) {
  qDebug() << error;
  if (error.contains("Token is not valid")) {
    tokenManager_->deleteToken();
  }
  userService_->deleteAll();
  emit noTokenFound();
}

void AuthService::disconnectFromServer() { network_->disconnectFromServer(); }