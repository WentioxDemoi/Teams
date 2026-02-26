#include "AuthViewModel.h"
#include "Interfaces/ISessionService.h"
#include "ServiceLocator.h"
#include "SessionService.h"

// Ici, dans le cas où on fait du testing, on passe en argument un objet construit localement pour les test. Sinon, en prod, on se base sur l'objet créé dans application.cpp
AuthViewModel::AuthViewModel(ISessionService *service, QObject *parent)
    : sessionService_(service ? service : ServiceLocator::instance().getService<ISessionService>()), QObject(parent)
{
  connect(sessionService_, &ISessionService::authSuccess,
          this, &AuthViewModel::authSuccess);
  connect(sessionService_, &ISessionService::authError,
          this, &AuthViewModel::authError);
  connect(sessionService_, &ISessionService::noTokenFound, this, &AuthViewModel::noTokenFound);
  connect(sessionService_, &ISessionService::registerWithServer4WebRTC, this, &AuthViewModel::registerWithServer4WebRTC);
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

