#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../../includes.h"
#include "../Models/User.h"
#include "NetworkService.h"

class AuthService : public QObject
{
    Q_OBJECT

public:
    explicit AuthService(NetworkService* network, QObject* parent = nullptr);

    void loginUser(const QString& username, const QString& password);
    void registerUser(const QString& username,
                      const QString& password);

signals:
    void authSuccess(const User& user);
    void authError(const QString& message);

private slots:
    void handleAuthResponse(const QByteArray& data);
    void handleNetworkError(const QString& error);

private:
    NetworkService* network_;
    QUrl baseUrl_ = QUrl("127.0.0.1:8080"); // adapte si besoin
};

#endif