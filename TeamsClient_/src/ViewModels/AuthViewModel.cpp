#include "AuthViewModel.h"

// Ici, dans le cas où on fait du testing, on passe en argument un objet construit localement pour les test. Sinon, en prod, on se base sur l'objet créé dans application.cpp
AuthViewModel::AuthViewModel(IAuthService *service, QObject *parent)
    : authService_(service ? service : ServiceLocator::instance().getService<IAuthService>()), QObject(parent)
{
  connect(authService_, &IAuthService::authSuccess,
          this, &AuthViewModel::authSuccess);
  connect(authService_, &IAuthService::authError,
          this, &AuthViewModel::loginError);
}

// Quelques cas d'erreurs avant redirection vers service
void AuthViewModel::loginUser(const QString &email, const QString &password)
{
  if (email.isEmpty() || password.isEmpty())
  {
    emit loginError("Username and password required");
    return;
  }

  authService_->loginUser(email, password);
}

// Quelques cas d'erreurs avant redirection vers service
void AuthViewModel::registerUser(const QString &email, const QString &username,
                                 const QString &password)
{
  if (email.isEmpty() || username.isEmpty() || password.isEmpty())
  {
    emit loginError("All fields are required");
    return;
  }

  if (!ViewModelsTools::isValidEmail(email))
  {
    emit loginError("Invalid email");
    return;
  }

  authService_->registerUser(email, username, password);
}