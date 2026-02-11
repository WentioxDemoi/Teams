#ifndef IUSERSERVICE_H
#define IUSERSERVICE_H

#include <QObject>
#include "User.h"

class IUserService : public QObject {
    Q_OBJECT
public:
    explicit IUserService(QObject *parent = nullptr) : QObject(parent) {};
    virtual ~IUserService() = default;

    public slots:
        virtual void saveUser(const User& user) = 0;
        virtual void deleteUser(QString uuid) = 0;
        virtual void deleteAll() = 0;

    signals:
        void userSaved(const User& user);
        void userDeleted(QString uuid);
        void error(const QString& error);
signals:
};

#endif