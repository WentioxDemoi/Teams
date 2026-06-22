#include "AuthViewModel.h"

#include "../Core/ServiceLocator.h"
#include "../Core/State/SessionState.h"
#include "../Core/State/UserState.h"
#include "../Core/StateLocator.h"
#include "../Services/Interfaces/IAuthService.h"
#include "ViewModelsTools.h"

// Ici, dans le cas où on fait du testing, on passe en argument un objet construit localement pour
// les test. Sinon, en prod, on se base sur l'objet créé dans application.cpp
AuthViewModel::AuthViewModel(IAuthService* authService, UserState* userState,
                             SessionState* sessionState, QObject* parent)
    : authService_(authService ? authService : ServiceLocator::instance().getService<IAuthService>()),
      userState_(userState ? userState : StateLocator::instance().getState<UserState>()),
      sessionState_(sessionState ? sessionState : StateLocator::instance().getState<SessionState>()),
      QObject(parent) {
  Q_ASSERT(authService_);
  Q_ASSERT(userState_);
  Q_ASSERT(sessionState_);

  connect(authService_, &IAuthService::authSuccess, this, &AuthViewModel::authSuccess);
  connect(authService_, &IAuthService::authError, this, &AuthViewModel::authError);
  connect(authService_, &IAuthService::noTokenFound, this, &AuthViewModel::noTokenFound);
  connect(authService_, &IAuthService::connectionUpdate, sessionState_, &SessionState::onServerConnectionUpdate);
  
  // Pour le connect sur le message received, il faudra faire en sorte de check si le user qui nous envoie le message est présent localement.
  // Dans le cas où il ne l'est pas, il faudra le créer et l'enregister dans la DB locale et sur serveur.
}

void AuthViewModel::start() {
  authService_->loginWithToken();
}

void AuthViewModel::loginUser(const QString& email, const QString& password) {
  if (email.isEmpty() || password.isEmpty()) {
    emit authError("Username and password required");
    return;
  }

  authService_->loginUser(email, password);
}

void AuthViewModel::registerUser(const QString& firstName, const QString& lastName, const QString& email,
                                 const QString& password) {
  if (email.isEmpty() || firstName.isEmpty() || lastName.isEmpty() || password.isEmpty()) {
    emit authError("All fields are required");
    return;
  }

  if (!ViewModelsTools::isValidEmail(email)) {
    emit authError("Invalid email");
    return;
  }

  authService_->registerUser(firstName, lastName, email, password);
}
