#ifndef AUTHVIEWMODELTEST_H
#define AUTHVIEWMODELTEST_H

#include <QtTest>
#include <QSignalSpy>
#include "AuthViewModel.h"
#include "IAuthService.h"
#include "User.h"

// FakeAuthService pour tests
class FakeAuthService : public IAuthService {
    Q_OBJECT
public:
    bool loginCalled = false;
    bool registerCalled = false;

    void loginUser(const QString&, const QString&) override { loginCalled = true; }
    void registerUser(const QString&, const QString&, const QString&) override { registerCalled = true; }

    void emitAuthSuccess(const User &user) { emit authSuccess(user); }
    void emitAuthError(const QString &error) { emit authError(error); }
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