#ifndef AUTHVIEWMODEL_H
#define AUTHVIEWMODEL_H

#include "../includes.h"
#include "../Models/User.h"

class AuthService;

class AuthViewModel : public QObject
{
    Q_OBJECT

public:
    explicit AuthViewModel(
                           QObject* parent = nullptr);

public slots:
    void login(const QString& username, const QString& password);
    void registerUser(const QString& username,
                      const QString& password);

signals:
    void loginSuccess(const User& user);
    void loginError(const QString& error);

private:
    AuthService* authService_;
};

#endif // AUTHVIEWMODEL_H