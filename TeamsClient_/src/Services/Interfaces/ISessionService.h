#ifndef ISESSIONSERVICE_H
#define ISESSIONSERVICE_H

#include <QObject>
#include "User.h"

class ISessionService : public QObject {
    Q_OBJECT
public:
    explicit ISessionService(QObject *parent = nullptr) : QObject(parent) {};
    virtual ~ISessionService() = default;

    virtual void loginUser(const QString &email, const QString &password) = 0;
    virtual void registerUser(const QString &email,
                              const QString &username,
                              const QString &password) = 0;
    virtual void start() = 0;

signals:
    void authSuccess(const User &user);
    void authError(const QString &error);
    void noTokenFound();
};

#endif