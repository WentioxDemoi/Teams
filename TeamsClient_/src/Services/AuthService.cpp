#include "AuthService.h"

AuthService::AuthService(QObject *parent) : QObject(parent) {
  network_ = ServiceLocator::instance().getService<AuthNetworkService>();
  connect(network_, &AuthNetworkService::authSuccess, this,
          &AuthService::authSuccess);
  connect(network_, &AuthNetworkService::authSuccess, this,
          &AuthService::saveUserData);

  connect(network_, &AuthNetworkService::authError, this,
          &AuthService::authError);
}

void AuthService::loginUser(const QString &email, const QString &password) {
  network_->loginUser(email, password);
}

void AuthService::registerUser(const QString &email, const QString &username,
                               const QString &password) {
  network_->registerUser(email, username, password);
}

void AuthService::saveUserData(const User &user) {
  // Accès DB pour enregistrer localement mon user
  qDebug() << "Save User Data";
}