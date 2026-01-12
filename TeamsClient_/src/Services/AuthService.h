#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../includes.h"
#include "../Models/User.h"
#include "AuthNetworkService.h"

class AuthService : public QObject {
    Q_OBJECT
    
    public:
    explicit AuthService(AuthNetworkService *network, QObject* parent = nullptr);

public slots:
    void loginUser(const QString& username,
                   const QString& password);

    void registerUser(const QString& username,
                      const QString& password);

signals:
    void authSuccess(const User& user);

    void authError(const QString& errorMessage);

private:
    AuthNetworkService *network_;
                             

};

#endif