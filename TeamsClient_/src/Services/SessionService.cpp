#include "SessionService.h"

#include "Auth/AuthService.h"
#include "Interfaces/ISessionService.h"
#include "Interfaces/ITokenManager.h"
#include "Interfaces/IUserService.h"
#include "ServiceLocator.h"
#include "State/SessionState.h"
#include "../Utils/TokenManager.h"


SessionService::SessionService(IAuthService* service, IUserService* userService,
                               ITokenManager* token, QObject* parent)
    : authService_(service ? service : ServiceLocator::instance().getService<IAuthService>()),
      userService_(userService ? userService
                               : ServiceLocator::instance().getService<IUserService>()),
      token_(token ? token : &TokenManager::instance()),
      sessionState_(new SessionState()),  // A transformer en singleton
      ISessionService(parent) {
  Q_ASSERT(authService_);
  connect(authService_, &AuthService::authSuccess, userService_, &IUserService::saveUser);
  connect(userService_, &IUserService::localUserSaved, this, &SessionService::onLocalUserSaved);
  connect(authService_, &AuthService::connectionUpdate, sessionState_, &SessionState::onServerConnectionUpdate);
  
  connect(authService_, &AuthService::authError, this, &SessionService::authError);
  connect (authService_, &AuthService::errorToken, this, &SessionService::errorToken);
}

void SessionService::start() {   
if (/*!*/ token_->token.isEmpty()) {
  //authService_->loginWithToken(token_->token);
  const QString token = "5abc486e2f1ba79f36a78cd475ade6b1";
  authService_->loginWithToken(token);
} else
  errorToken("No token found"); }

void SessionService::loginUser(const QString& username, const QString& password) {
  authService_->loginUser(username, password);
}

void SessionService::registerUser(const QString& firstName, const QString& lastName,
                                  const QString& email, const QString& password) {
  authService_->registerUser(firstName, lastName, email, password);
}

void SessionService::onLocalUserSaved(const User& user) {
  User localUser = user;

  if (!user.token().isEmpty()) {
    token_->writeToken(localUser.token());
    localUser.clearToken();
    emit authSuccess(localUser);
  } else {
    qDebug() << "No Token in the user";
  }
}

// Ici on doit delete le user et session State
void SessionService::errorToken(const QString& error) {
  qDebug() << error;
  if (error.contains("Token is not valid")) 
    token_->deleteToken();

  userService_->deleteAll();
  emit noTokenFound();
}