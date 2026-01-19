#include "AuthService.h"

AuthService::AuthService(QObject *parent) : IAuthService(parent)
{
  network_ = ServiceLocator::instance().getService<AuthNetworkService>();
  db_ = ServiceLocator::instance().getService<DBService>();
  token_ = new TokenManager();
  initTokenAuth(); // Ici on va directement lancer une requete pour check si le token est valide
  initClassicAuth();
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

void AuthService::initTokenAuth()
{
  connect(token_, &TokenManager::error, this, &AuthService::errorTokenManager);
  connect(token_, &TokenManager::keyRestored, network_, &AuthNetworkService::validateToken);
  token_->readKey("Token");
}

void AuthService::initClassicAuth()
{
  connect(network_, &AuthNetworkService::authSuccess, db_,
          &DBService::saveUser);
  connect(db_, &DBService::userSaved, this, &AuthService::onUserSaved);

  connect(network_, &AuthNetworkService::authError, this,
          &AuthService::authError);
}

void AuthService::onUserSaved(const User &user)
{
  User localUser = user;
  if (user.token() != nullptr)
  {
    token_->writeKey("Token", localUser.token());
    localUser.clearToken();
    emit authSuccess(localUser);
  }
  else
  {
    qDebug() << "Pas de token dans le user (Ligne 46 AuthService)";
  }
}

void AuthService::errorTokenManager(const QString &errorText)
{
  if (errorText.contains("Read key failed: Could not retrieve private key from keystore: The specified item could not be found in the keychain"))
  {
    qDebug() << errorText;
    initClassicAuth();
    // Supprimer quand même la DB locale pour plus de sécurité
    emit noTokenFound();
  }
}