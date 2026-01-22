#include "UserService.h"

UserService::UserService(QObject* parent) : QObject(parent) {
    repo_ = new UserRepository();
    qDebug() << "Repo instance crée";
}

void UserService::saveUser(const User& user) {
    qDebug() << "aller";
    if (repo_->insert(user))
        emit userSaved(user);
    else {
        qDebug() << "erreur";
        emit error("Failed to save user.");
    }
}
    
void UserService::deleteUser(QString uuid) {

    repo_->remove(uuid);
    emit userDeleted(uuid);
}

// signals:
//     void userSaved(const User&);
//     void userDeleted(int id);
//     void error(const QString&);