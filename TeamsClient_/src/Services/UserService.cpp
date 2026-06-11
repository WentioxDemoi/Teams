#include "UserService.h"
#include "Interfaces/IUserService.h"
#include "Repositories/UserRepository.h"
#include "State/UserState.h"
#include "StateLocator.h"

UserService::UserService(UserRepository* userRepo, UserState *userState, QObject* parent)
    : userRepo_(userRepo ? userRepo : new UserRepository(parent)), 
    userState_(userState ? userState : StateLocator::instance().getState<UserState>()),
    IUserService(parent) {
      connect(this, &IUserService::saveLocalUser, userState_, &UserState::saveLocalUser);
      connect(userState_, &UserState::localUserSaved, this, &UserService::localUserSaved);

    }

void UserService::saveUser(const User& user) {
  if (user.isMe()) {
    emit saveLocalUser(user);
  } else if (userRepo_->save(user))
    emit userSaved(user);
  else {
    qDebug() << "Error lors de l'enregistrement du user : " + user.uuid() + ".";
  }
}

void UserService::deleteUser(QString uuid) {
  if (userRepo_->remove(uuid)) {
    qDebug() << "Utilisateur supprimé : " + uuid;
  } else {
    qDebug() << "Erreur lors de la suppression du user : " + uuid + ".";
  }
}

void UserService::deleteAll() {
  if (userRepo_->removeAll()) {
    qDebug() << "Tous les utilisateurs supprimés.";
  } else {
    qDebug() << "Erreur lors de la suppression de tous les users.";
  }
}