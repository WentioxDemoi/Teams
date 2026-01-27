#include "SessionServiceTest.h"

void SessionServiceTest::start_calls_authService() {
    MockAuthService mock;
    SessionService service(&mock);

    service.start();
    QVERIFY(mock.startCalled);
}

void SessionServiceTest::loginUser_delegates() {
    MockAuthService mock;
    SessionService service(&mock);

    service.loginUser("test@mail.com", "pwd");
    QCOMPARE(mock.lastEmail, QString("test@mail.com"));
    QCOMPARE(mock.lastPassword, QString("pwd"));
}

void SessionServiceTest::registerUser_delegates() {
    MockAuthService mock;
    SessionService service(&mock);

    service.registerUser("a@b.com", "user", "pwd");
    QCOMPARE(mock.lastEmail, QString("a@b.com"));
    QCOMPARE(mock.lastUsername, QString("user"));
}

void SessionServiceTest::relay_authSuccess() {
    MockAuthService mock;
    SessionService service(&mock);

    QSignalSpy spy(&service, &SessionService::authSuccess);

    User user;
    emit mock.authSuccess(user);

    QCOMPARE(spy.count(), 1);
}
