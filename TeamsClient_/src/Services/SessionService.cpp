#include "SessionService.h"

#include "AuthService.h"
#include "ServiceLocator.h"

SessionService::SessionService(IAuthService* service, QObject* parent)
    : authService_(service ? service : ServiceLocator::instance().getService<IAuthService>()),
      ISessionService(parent) {
  Q_ASSERT(authService_);
  connect(authService_, &AuthService::authSuccess, this, &SessionService::authSuccess);
  connect(authService_, &AuthService::authError, this, &SessionService::authError);
  connect(authService_, &AuthService::noTokenFound, this, &SessionService::noTokenFound);
  connect(authService_, &AuthService::registerWithServer4WebRTC, this,
          &SessionService::registerWithServer4WebRTC);
}

void SessionService::start() { authService_->start(); }

void SessionService::loginUser(const QString& username, const QString& password) {
  authService_->loginUser(username, password);
}

void SessionService::registerUser(const QString& email, const QString& username,
                                  const QString& password) {
  authService_->registerUser(email, username, password);
}
