#include "AuthService.h"
#include "ServiceLocator.h"

#include <cstdlib>
#include "State/SessionState.h"
#include "TokenManager.h"

#include "Network/NetworkService.h"

AuthService::AuthService(NetworkService* network, ILocalUserService* localUserService, IMessageService* messageService,
                         ITokenManager* tokenManager, QObject* parent)
    : IAuthService(parent),
      network_(network ? network : new NetworkService(8080, parent)),
      localUserService_(localUserService ? localUserService : ServiceLocator::instance().getService<ILocalUserService>()),
      messageService_(messageService ? messageService : ServiceLocator::instance().getService<IMessageService>()),
      tokenManager_(tokenManager ? tokenManager : &TokenManager::instance()) {
  Q_ASSERT(network_);
  Q_ASSERT(localUserService_);
  Q_ASSERT(tokenManager_);

  connect(network_, &NetworkService::networkError, this, &IAuthService::authError);
  connect(network_, &NetworkService::connectionUpdate, &SessionState::instance(),  &SessionState::onServerConnectionUpdate);
  connect(network_, &NetworkService::jsonReceived, this, &AuthService::handleServerResponse);

  connect(this, &IAuthService::errorToken, this, &AuthService::handleTokenError);

  connect(localUserService_, &ILocalUserService::localUserSaved, this, &AuthService::handleLocalUserSaved);
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
  // Temporaire pour. éviter d'avoir à tapper le code du PC à chaque démarrage pour que l'app ait accès au token
  if (/*!*/tokenManager_->token.isEmpty()) {
    //network_->send({{"type", "validate_token"}, {"token", tokenManager_->token}});
    network_->send({{"type", "validate_token"}, {"token", "2a3841a340ac4f683c34e03e6b033822e"}});
  } else {
      handleTokenError("No token found");
  }
}

void AuthService::handleServerResponse(const QJsonObject& root) {
  const QString type = root["type"].toString();

  if (root.contains("error") && root["error"].isString()) {
    const QString error = root["error"].toString();

    if (type == "validate_token_response" && error.contains("invalid token", Qt::CaseInsensitive)) {
      emit errorToken("Token is not valid");
      return;
    }

    emit authError(error);
    return;
  }

  if (type != "login_response" && type != "register_response" &&
      type != "validate_token_response") {
    qDebug() << "[AuthService] Unknown type.\n";
    return;
  }

  if (!root.contains("data") || !root["data"].isObject()) {
    emit authError("Missing data field in server response");
    return;
  }

  const User user = User::fromJson(root["data"].toObject());

  if (!user.isValid()) {
    emit authError("Invalid user data received from server");
    return;
  }

  if (user.token().isEmpty()) {
    emit authError("No token received from server");
    return;
  }

  User me = user;
  me.setIsMe(true);
  tokenManager_->writeToken(me.token());
  localUserService_->saveLocalUser(me);
}

void AuthService::handleLocalUserSaved(const User& user) {
  qDebug() << "AuthService::handleLocalUserSaved called for" << user.uuid();
  User localUser = user;
  localUser.clearToken();
  emit authSuccess(localUser);
}

void AuthService::handleTokenError(const QString& error) {
  qDebug() << error;
  if (error.contains("Token is not valid")) {
    tokenManager_->deleteToken();
  }
  localUserService_->deleteAll(); // Pour l'instant on supprime tous les contacts via localUserService. A terme il faudrait que chaque service supprimer ce qu'il enregistre.
  messageService_->deleteAll();
  emit noTokenFound();
}

void AuthService::disconnectFromServer() { network_->disconnectFromServer(); }