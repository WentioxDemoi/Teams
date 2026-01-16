#include "AuthService.h"

AuthService::AuthService(QObject *parent) : IAuthService(parent)
{
  network_ = ServiceLocator::instance().getService<AuthNetworkService>();
  db_ = ServiceLocator::instance().getService<DBService>();
  token_ = new TokenManager();
  // connect(network_, &AuthNetworkService::authSuccess, this,
  //         &AuthService::authSuccess);
  connect(network_, &AuthNetworkService::authSuccess, db_,
          &DBService::saveUser);
  connect(db_, &DBService::userSaved, this, &AuthService::authSuccess);

  connect(network_, &AuthNetworkService::authError, this,
          &AuthService::authError);
  connect(token_, &TokenManager::error, this, &AuthService::errorTokenManager);

  // Ici on va directement lancer une requete pour check si le token est valide
}

void AuthService::loginUser(const QString &email, const QString &password)
{
  network_->loginUser(email, password);
}

void AuthService::registerUser(const QString &email, const QString &username,
                               const QString &password)
{
  network_->registerUser(email, username, password);
}

void AuthService::errorTokenManager(const QString &errorText)
{
  qDebug() << errorText;
}