#ifndef AUTHVIEWMODELTEST_H
#define AUTHVIEWMODELTEST_H

#include <QtTest>
#include <QSignalSpy>
#include "AuthViewModel.h"
#include "IAuthService.h"
#include "User.h"

class FakeSessionService : public ISessionService {
    Q_OBJECT
public:
    bool loginCalled = false;
    bool registerCalled = false;
    bool startCalled = false;

    void loginUser(const QString&, const QString&) override {
        loginCalled = true;
    }

    void registerUser(const QString&, const QString&, const QString&) override {
        registerCalled = true;
    }

    void start() override {
        startCalled = true;
    }

    // Helpers pour tests
    void emitAuthSuccess(const User &user) { emit authSuccess(user); }
    void emitAuthError(const QString &error) { emit authError(error); }
    void emitNoTokenFound() { emit noTokenFound(); }
};

class AuthViewModelTest : public QObject {
    Q_OBJECT
private slots:
    void testLoginEmptyFieldsEmitsError();
    void testLoginValidCallsService();
    void testRegisterEmptyFieldsEmitsError();
    void testRegisterValidCallsService();
    void testAuthSuccessIsPropagated();
    void testAuthErrorIsPropagated();
};

#endif