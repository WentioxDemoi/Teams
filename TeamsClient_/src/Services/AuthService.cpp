#include "AuthService.h"

AuthService::AuthService(AuthNetworkService* network,
                         UserService* userService,
                         TokenManager* token,
                         QObject* parent)
    : IAuthService(parent),
      network_(network ? network : ServiceLocator::instance().getService<AuthNetworkService>()),
      userService_(userService ? userService : ServiceLocator::instance().getService<UserService>()),
      token_(token ? token : &TokenManager::instance())
{
    Q_ASSERT(network_);
    Q_ASSERT(userService_);
    Q_ASSERT(token_);

    connect(network_, &AuthNetworkService::authSuccess, userService_,
            &UserService::saveUser);
    connect(userService_, &UserService::userSaved, this,
            &AuthService::onUserSaved);

    connect(network_, &AuthNetworkService::authError, this,
            &AuthService::authError);
    connect(network_, &AuthNetworkService::invalidToken, this,
            &AuthService::errorToken);
    connect(userService_, &UserService::error, this,
            &AuthService::errorUserService);
}

void AuthService::start() {
  if (token_->token != nullptr)
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