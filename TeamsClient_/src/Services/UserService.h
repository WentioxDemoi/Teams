#ifndef USERSERVICE_H
#define USERSERVICE_H

#include "../includes.h"
#include "../Repositories/UserRepository.h"
#include "../Models/User.h"
#include "Interfaces/IUserService.h"

class UserService : public IUserService
{
    Q_OBJECT
public:
    explicit UserService(QObject* parent = nullptr);

public slots:
    void saveUser(const User& user) override;
    void deleteUser(QString uuid) override;
    void deleteAll() override;


private:
    UserRepository *repo_;
};

#endif