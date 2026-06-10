#include "UserService.h"
#include "Interfaces/IUserService.h"
#include "Repositories/UserRepository.h"
#include "State/UserState.h"
#include "StateLocator.h"

UserService::UserService(UserRepository* repo, UserState *userState, QObject* parent)
    : repo_(repo ? repo : new UserRepository()), 
    userState_(userState ? userState : StateLocator::instance().getState<UserState>()),
    IUserService(parent) {
      connect(this, &IUserService::saveLocalUser, userState_, &UserState::saveLocalUser);
      connect(userState_, &UserState::localUserSaved, this, &UserService::localUserSaved);

    }

void UserService::saveUser(const User& user) {
  if (user.isMe()) {
    emit saveLocalUser(user);
  } else if (repo_->insert(user))
    emit userSaved(user);
  else {
    qDebug() << "Error lors de l'enregistrement du user : " + user.uuid() + ".";
  }
}

void UserService::deleteUser(QString uuid) {
  if (repo_->remove(uuid)) {
    qDebug() << "Utilisateur supprimé : " + uuid;
  } else {
    qDebug() << "Erreur lors de la suppression du user : " + uuid + ".";
  }
}

void UserService::deleteAll() {
  if (repo_->removeAll()) {
    qDebug() << "Tous les utilisateurs supprimés.";
  } else {
    qDebug() << "Erreur lors de la suppression de tous les users.";
  }
}