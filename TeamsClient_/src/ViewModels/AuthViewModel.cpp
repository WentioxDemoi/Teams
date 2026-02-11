#include "AuthViewModel.h"

// Ici, dans le cas où on fait du testing, on passe en argument un objet construit localement pour les test. Sinon, en prod, on se base sur l'objet créé dans application.cpp
AuthViewModel::AuthViewModel(ISessionService *service, QObject *parent)
    : sessionService_(service ? service : new SessionService(nullptr, parent)), QObject(parent)
{
  connect(sessionService_, &SessionService::authSuccess,
          this, &AuthViewModel::authSuccess);
  connect(sessionService_, &SessionService::authError,
          this, &AuthViewModel::authError);
  connect(sessionService_, &SessionService::noTokenFound, this, &AuthViewModel::noTokenFound);
  }
  
void AuthViewModel::start()
{
    sessionService_->start();
}

void AuthViewModel::loginUser(const QString &email, const QString &password)
{
  if (email.isEmpty() || password.isEmpty())
  {
    emit authError("Username and password required");
    return;
  }

  sessionService_->loginUser(email, password);
}

void AuthViewModel::registerUser(const QString &email, const QString &username,
                                 const QString &password)
{
  if (email.isEmpty() || username.isEmpty() || password.isEmpty())
  {
    emit authError("All fields are required");
    return;
  }

  if (!ViewModelsTools::isValidEmail(email))
  {
    emit authError("Invalid email");
    return;
  }

  sessionService_->registerUser(email, username, password);
}

