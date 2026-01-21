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
  connect(token_, &TokenManager::error, this, &AuthService::errorToken);
  connect(token_, &TokenManager::keyRestored, network_, &AuthNetworkService::validateToken);
  connect(network_, &AuthNetworkService::invalidToken, this, &AuthService::errorToken);
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
    qDebug() << "\n=== onUserSaved START ===";
    qDebug() << "User received in onUserSaved:";
    user.print();

    User localUser = user;

    // Vérification du token
    if (!user.token().isEmpty()) {
        qDebug() << "Token trouvé dans le user:" << user.token();

        token_->writeKey("Token", localUser.token());
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

void AuthService::errorToken(const QString &errorText)
{
  if (errorText.contains("Read key failed") && errorText.contains("could not be found in the keychain"))
  {
    qDebug() << errorText;
    // Supprimer quand même la DB locale pour plus de sécurité
    
  }
  else if (errorText.contains("Token is not valid")) {
    token_->deleteKey("Token");
  }
  emit noTokenFound();
}