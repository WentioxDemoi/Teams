#include "AuthService.h"

AuthService::AuthService(QObject *parent) : IAuthService(parent) {
  network_ = ServiceLocator::instance().getService<AuthNetworkService>();
  userService_ = ServiceLocator::instance().getService<UserService>();
  token_ = &TokenManager::instance();
}

void AuthService::start() {
  connect(network_, &AuthNetworkService::authSuccess, userService_,
          &UserService::saveUser);
  connect(userService_, &UserService::userSaved, this,
          &AuthService::onUserSaved);

  connect(network_, &AuthNetworkService::authError, this,
          &AuthService::authError);
  connect(network_, &AuthNetworkService::invalidToken, this,
          &AuthService::errorToken);
  if (token_->token != nullptr)

    network_->validateToken(token_->token); // check si le token est valide
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
  qDebug() << "\n=== onUserSaved START ===";
  qDebug() << "User received in onUserSaved:";
  user.print();

  User localUser = user;

  // Vérification du token
  if (!user.token().isEmpty()) {
    qDebug() << "Token trouvé dans le user:" << user.token();

    token_->writeToken(localUser.token());
    qDebug() << "Token écrit dans le store, suppression locale du token";

    localUser.clearToken();
    qDebug() << "Token après clearToken(): '" << localUser.token() << "'";

    emit authSuccess(localUser);
    qDebug() << "authSuccess émis avec l'utilisateur (token supprimé)";
  } else {
    qDebug() << "⚠️ Pas de token dans le user (Ligne 46 AuthService)";
  }

  qDebug() << "=== onUserSaved END ===\n";
}

void AuthService::errorToken(const QString &errorText) {
  if (errorText.contains("No token found")) {
    qDebug() << errorText;
    // Supprimer quand même la DB locale pour plus de sécurité

  } else if (errorText.contains("Token is not valid")) {
    token_->deleteToken();
  }
  emit noTokenFound();
}