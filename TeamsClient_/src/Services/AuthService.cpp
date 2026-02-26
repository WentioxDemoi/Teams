#include "AuthService.h"
#include "Interfaces/IAuthNetworkService.h"
#include "ServiceLocator.h"
#include "WebRTCService.h"

AuthService::AuthService(IAuthNetworkService* network,
                         IUserService* userService,
                         ITokenManager* token,
                         QObject* parent)
    : IAuthService(parent),
      network_(network ? network : ServiceLocator::instance().getService<IAuthNetworkService>()),
      userService_(userService ? userService : ServiceLocator::instance().getService<IUserService>()),
      token_(token ? token : &TokenManager::instance())
{
    Q_ASSERT(network_);
    Q_ASSERT(userService_);
    Q_ASSERT(token_);

    connect(network_, &IAuthNetworkService::authSuccess, userService_,
            &IUserService::saveUser);
    connect(userService_, &IUserService::userSaved, this,
            &IAuthService::onUserSaved);

    connect(network_, &IAuthNetworkService::authError, this,
            &IAuthService::authError);
    connect(network_, &IAuthNetworkService::invalidToken, this,
            &IAuthService::errorToken);
    connect(userService_, &IUserService::error, this,
            &IAuthService::errorUserService);
    connect(network_, &IAuthNetworkService::registerWithServer4WebRTC, this, &IAuthService::registerWithServer4WebRTC);
}

void AuthService::start() {
  if (!token_->token.isEmpty())
    network_->validateToken(token_->token);
  else
    errorToken("No token found");
}

void AuthService::loginUser(const QString &email, const QString &password) {
  network_->loginUser(email, password);
}

void AuthService::registerUser(const QString &email, const QString &username,
                               const QString &password) {
  network_->registerUser(email, username, password);
}

void AuthService::onUserSaved(const User &user) {
  User localUser = user;

  if (!user.token().isEmpty()) {
    token_->writeToken(localUser.token());
    localUser.clearToken();
    emit authSuccess(localUser);
  } else {
    qDebug() << "Pas de token dans le user (Ligne 46 AuthService)";
  }
}

void AuthService::errorToken(const QString &error) {
  qDebug() << error;
  if (error.contains("Token is not valid"))
    token_->deleteToken();

  userService_->deleteAll();
  emit noTokenFound();
}

void AuthService::errorUserService(const QString &error) { qDebug() << error; }