#include "AuthViewModel.h"

AuthViewModel::AuthViewModel(QObject *parent) : QObject(parent) {
  authService_ = ServiceLocator::instance().getService<AuthService>();
  connect(authService_, &AuthService::authSuccess, this,
          &AuthViewModel::authSuccess);

  connect(authService_, &AuthService::authError, this,
          &AuthViewModel::loginError);
}

void AuthViewModel::loginUser(const QString &email, const QString &password) {
  if (email.isEmpty() || password.isEmpty()) {
    emit loginError("Username and password required");
    return;
  }

  authService_->loginUser(email, password);
}

void AuthViewModel::registerUser(const QString &email, const QString &username,
                                 const QString &password) {
  if (email.isEmpty() || username.isEmpty() || password.isEmpty()) {
    emit loginError("All fields are required");
    return;
  }

  // if (!Validators::isValidEmail(email)) {
  //     emit loginError("Invalid email");
  //     return;
  // }

  authService_->registerUser(email, username, password);
}