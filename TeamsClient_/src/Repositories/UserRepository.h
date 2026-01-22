#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include "../includes.h"
#include "../Models/User.h"
#include "../Database/DatabaseManager.h"

class UserRepository : public QObject
{
    Q_OBJECT
public:
    explicit UserRepository(QObject *parent = nullptr);

    std::optional<User> findByUUID(const QString &uuid);
    QList<User> findAll();
    
    bool insert(const User& user);
    bool remove(const QString &uuid);
    bool isUserPresent(const QString &uuid);
    bool update(const User &user);
    bool isFirstUser();

signals:
    void error(const QString& message);

private:
    QSqlDatabase db_;
};

#endif