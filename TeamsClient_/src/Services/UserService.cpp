#include "UserService.h"

UserService::UserService(QObject* parent) : IUserService(parent) {
    repo_ = new UserRepository();
}

void UserService::saveUser(const User& user) {
    if (repo_->insert(user))
        emit userSaved(user);
    else {
        emit error("Error lors de l'enregistrement du user : " + user.uuid() + ".");
    }
}
    
void UserService::deleteUser(QString uuid) {

    if (repo_->remove(uuid)) {
        // emit userDeleted(uuid); Pas encore utilisé
        qDebug() << "Utilisateur supprimé : " + uuid;
    }
    else
        emit error("Erreur lors de la suppression du user : " + uuid + ".");
}

void UserService::deleteAll()
{
    if (repo_->removeAll()) {
        qDebug() << "Tous les utilisateurs supprimés.";
    } else 
        emit error("Erreur lors de la suppression de tous les users.");
}