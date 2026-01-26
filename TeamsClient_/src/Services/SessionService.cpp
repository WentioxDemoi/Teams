#include "SessionService.h"

SessionService::SessionService(IAuthService* service, QObject* parent)
    : authService_(service ? service : new AuthService(nullptr, nullptr, nullptr, parent)),
      ISessionService(parent)
{
    Q_ASSERT(authService_);

    connect(authService_, &AuthService::authSuccess,
            this, &ISessionService::authSuccess);
    connect(authService_, &AuthService::authError,
            this, &ISessionService::authError);
    connect(authService_, &AuthService::noTokenFound,
            this, &ISessionService::noTokenFound);
}

void SessionService::start() { authService_->start(); }

void SessionService::loginUser(const QString &username,
                               const QString &password) {
  authService_->loginUser(username, password);
}

void SessionService::registerUser(const QString &email, const QString &username,
                                  const QString &password) {
  authService_->registerUser(email, username, password);
}
