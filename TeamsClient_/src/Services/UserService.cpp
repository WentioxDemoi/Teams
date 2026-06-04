#include "UserService.h"

#include "Interfaces/IUserService.h"
#include "Repositories/UserRepository.h"
#include "State/UserState.h"

UserService::UserService(UserRepository* repo, QObject* parent)
    : repo_(repo ? repo : new UserRepository()), 
    userState_(&UserState::instance()),
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
    // emit error("Error lors de l'enregistrement du user : " + user.uuid() + ".");
    qDebug() << "Error lors de l'enregistrement du user : " + user.uuid() + ".";
  }
}

void UserService::deleteUser(QString uuid) {
  if (repo_->remove(uuid)) {
    // emit userDeleted(uuid); Pas encore utilisé
    qDebug() << "Utilisateur supprimé : " + uuid;
  } else {
    // emit error("Erreur lors de la suppression du user : " + uuid + ".");
    qDebug() << "Erreur lors de la suppression du user : " + uuid + ".";
  }
}

void UserService::deleteAll() {
  if (repo_->removeAll()) {
    qDebug() << "Tous les utilisateurs supprimés.";
  } else {
    // emit error("Erreur lors de la suppression de tous les users.");
    qDebug() << "Erreur lors de la suppression de tous les users.";
  }
}