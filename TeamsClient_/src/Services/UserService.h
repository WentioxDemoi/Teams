#ifndef USERSERVICE_H
#define USERSERVICE_H

#include "../includes.h"
#include "../Repositories/UserRepository.h"
#include "../Models/User.h"

class UserService : public QObject
{
    Q_OBJECT
public:
    explicit UserService(QObject* parent = nullptr);

public slots:
    void saveUser(const User& user);
    void deleteUser(QString uuid);

signals:
    void userSaved(const User& user);
    void userDeleted(QString uuid);
    void error(const QString& error);

private:
    UserRepository *repo_;
};

#endif