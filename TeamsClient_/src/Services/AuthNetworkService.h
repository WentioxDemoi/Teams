#ifndef AUTHNETWORKSERVICE_H
#define AUTHNETWORKSERVICE_H

#include "../includes.h"
#include "../Models/User.h"

class AuthNetworkService : public QObject
{
    Q_OBJECT
public:
    explicit AuthNetworkService(QObject* parent = nullptr);

    void login(const QString& username, const QString& password);
    void registerUser(const QString& email, const QString& username, const QString& password);

signals:
    void authSuccess(const User& user);
    void authError(const QString& message);

private:
    void sendRequest(const QJsonObject& payload);

    QSslSocket socket_;
};

#endif