#include "LocalUserService.h"
#include "Repositories/UserRepository.h"
#include "State/UserState.h"
#include "StateLocator.h"

LocalUserService::LocalUserService(UserState *userState, UserRepository *userRepo, QObject *parent)
    : ILocalUserService(parent), userRepo_ (userRepo ? userRepo : new UserRepository()),
      userState_(userState ? userState : StateLocator::instance().getState<UserState>()) {
  connect(userState_, &UserState::localUserSaved, this, &ILocalUserService::localUserSaved);
  qDebug() << "LocalUserService" << this << "connected to UserState" << userState_;
}

void LocalUserService::saveLocalUser(const User &user) {
  userState_->saveLocalUser(user); 
  // Actuellement ce n'est pas une sauvegarde en DB car de toute façon le User est envoyé à chaque démarrage car on envoie toujours le token au serveur.
  // Il faudrait faire en sorte de vérifier localement que le token est encore valide pour pouvoir se connecter OFFLINE
}

void LocalUserService::deleteAll() {
  if (userRepo_.removeAll()) {
    qDebug() << "Tous les utilisateurs supprimés.";
  } else {
    qDebug() << "Erreur lors de la suppression de tous les users.";
  }
}