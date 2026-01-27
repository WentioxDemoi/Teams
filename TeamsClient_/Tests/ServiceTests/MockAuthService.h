#ifndef MOCKAUTHSERVICE_H
#define MOCKAUTHSERVICE_H

#include <QtTest>
#include "SessionService.h"

class MockAuthService : public IAuthService {
    Q_OBJECT
public:
    bool startCalled = false;
    QString lastEmail, lastUsername, lastPassword;

    explicit MockAuthService(QObject *parent = nullptr)
        : IAuthService(parent) {}

    void start() override { startCalled = true; }

    void loginUser(const QString &email, const QString &password) override {
        lastEmail = email;
        lastPassword = password;
    }

    void registerUser(const QString &email,
                      const QString &username,
                      const QString &password) override {
        lastEmail = email;
        lastUsername = username;
        lastPassword = password;
    }
public slots : 
  void onUserSaved(const User &user) override {};
  void errorToken(const QString &error) override {};
  void errorUserService(const QString &error) override {};
};

#endif