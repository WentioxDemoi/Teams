#include "AuthViewModelTest.h"

void AuthViewModelTest::testLoginEmptyFieldsEmitsError() {
    FakeSessionService service;
    AuthViewModel vm(&service);

    bool errorEmitted = false;
    QString capturedError;

    connect(&vm, &AuthViewModel::authError,
            [&](const QString &error){ errorEmitted = true; capturedError = error; });

    vm.loginUser("", "");

    QVERIFY(errorEmitted);
    QCOMPARE(capturedError, QString("Username and password required"));
    QVERIFY(!service.loginCalled);
}

void AuthViewModelTest::testLoginValidCallsService() {
    FakeSessionService service;
    AuthViewModel vm(&service);

    bool errorEmitted = false;
    connect(&vm, &AuthViewModel::authError,
            [&](const QString &){ errorEmitted = true; });

    vm.loginUser("test@mail.com", "1234");

    QVERIFY(!errorEmitted);
    QVERIFY(service.loginCalled);
}

void AuthViewModelTest::testRegisterEmptyFieldsEmitsError() {
    FakeSessionService service;
    AuthViewModel vm(&service);

    bool errorEmitted = false;
    QString capturedError;

    connect(&vm, &AuthViewModel::authError,
            [&](const QString &error){ errorEmitted = true; capturedError = error; });

    vm.registerUser("", "", "");

    QVERIFY(errorEmitted);
    QCOMPARE(capturedError, QString("All fields are required"));
    QVERIFY(!service.registerCalled);
}

void AuthViewModelTest::testRegisterValidCallsService() {
    FakeSessionService service;
    AuthViewModel vm(&service);

    bool errorEmitted = false;
    connect(&vm, &AuthViewModel::authError,
            [&](const QString &){ errorEmitted = true; });

    vm.registerUser("email@mail.com", "username", "password");

    QVERIFY(!errorEmitted);
    QVERIFY(service.registerCalled);
}

void AuthViewModelTest::testAuthSuccessIsPropagated()
{
    FakeSessionService service;
    AuthViewModel vm(&service);

    QSignalSpy spy(&vm, &AuthViewModel::authSuccess);

    User user(0, "u@mail.com", "user", "token", "active");
    service.emitAuthSuccess(user);

    QCOMPARE(spy.count(), 1);

    const auto arguments = spy.takeFirst();
    User receivedUser = arguments.at(0).value<User>();

    QCOMPARE(receivedUser.email(), user.email());
}

void AuthViewModelTest::testAuthErrorIsPropagated()
{
    FakeSessionService service;
    AuthViewModel vm(&service);

    QSignalSpy spy(&vm, &AuthViewModel::authError);

    service.emitAuthError("fail");

    QCOMPARE(spy.count(), 1);

    const auto arguments = spy.takeFirst();
    QString error = arguments.at(0).toString();

    QCOMPARE(error, QString("fail"));
}