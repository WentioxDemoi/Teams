#ifndef DBSERVICE_H
#define DBSERVICE_H

#include "../../includes.h"
#include "../../Models/User.h"
#include <QDir>
#include <QFile>
#include <QSqlDriver>

class DBService : public QObject
{
    Q_OBJECT
public:
    explicit DBService(QObject *parent = nullptr);

    void saveUser(const User &user);
    void loadUser(int id);
    void deleteUser(int id);
    void clearUsers();
    void showAllUsers();

signals:
    void userSaved(const User &user);
    void userLoaded(const User &user);
    void userDeleted(int id);
    void dbError(const QString &error);

private:
    QSqlDatabase db_;
};

#endif