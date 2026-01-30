#include "AuthServiceTest.h"
#include "User.h"
#include <QtTest>

void AuthServiceTest::start_withToken_callsValidate() {
    FakeAuthNetworkService network;
    FakeUserServiceFull userService;
    FakeTokenManager token;
    token.token = "abc";

    AuthService service(&network, &userService, &token);

    service.start();

    QCOMPARE(network.lastValidatedToken, QString("abc"));
}

void AuthServiceTest::start_withoutToken_emitsNoTokenFound() {
    FakeAuthNetworkService network;
    FakeUserServiceFull userService;
    FakeTokenManager token; // token vide

    AuthService service(&network, &userService, &token);
    QSignalSpy spy(&service, &AuthService::noTokenFound);

    service.start();

    QCOMPARE(spy.count(), 1);
}

void AuthServiceTest::loginUser_delegates() {
    FakeAuthNetworkService network;
    FakeUserServiceFull userService;
    FakeTokenManager token;

    AuthService service(&network, &userService, &token);
    service.loginUser("a@b.com", "pwd");

    QCOMPARE(network.lastEmail, QString("a@b.com"));
    QCOMPARE(network.lastPassword, QString("pwd"));
}

void AuthServiceTest::registerUser_delegates() {
    FakeAuthNetworkService network;
    FakeUserServiceFull userService;
    FakeTokenManager token;

    AuthService service(&network, &userService, &token);
    service.registerUser("a@b.com", "user", "pwd");

    QCOMPARE(network.lastRegisterEmail, QString("a@b.com"));
    QCOMPARE(network.lastRegisterUsername, QString("user"));
    QCOMPARE(network.lastRegisterPassword, QString("pwd"));
}

void AuthServiceTest::onUserSaved_withToken_emitsAuthSuccess() {
    FakeAuthNetworkService network;
    FakeUserServiceFull userService;
    FakeTokenManager token;

    AuthService service(&network, &userService, &token);
    QSignalSpy spy(&service, &AuthService::authSuccess);

    User user;
    user.setId(1);
    user.setEmail("a@b.com");
    user.setUsername("user");
    user.setStatus("active");
    user.setUuid("uuid-123");
    user.setToken("abc");

    service.onUserSaved(user);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(token.token, QString("abc"));
}

void AuthServiceTest::onUserSaved_withoutToken_doesNotEmit() {
    FakeAuthNetworkService network;
    FakeUserServiceFull userService;
    FakeTokenManager token;

    AuthService service(&network, &userService, &token);
    QSignalSpy spy(&service, &AuthService::authSuccess);

    User user;
    user.setId(1);
    user.setEmail("a@b.com");
    user.setUsername("user");
    user.setStatus("active");
    user.setUuid("uuid-123");

    service.onUserSaved(user);

    QCOMPARE(spy.count(), 0);
}

void AuthServiceTest::errorToken_invalidToken_clearsTokenAndDeletesUser() {
    FakeAuthNetworkService network;
    FakeUserServiceFull userService;
    FakeTokenManager token;
    token.token = "abc";

    AuthService service(&network, &userService, &token);
    QSignalSpy spy(&service, &AuthService::noTokenFound);

    service.errorToken("Token is not valid");

    QVERIFY(token.deleteCalled);
    QVERIFY(userService.deleteAllCalled);
    QCOMPARE(spy.count(), 1);
}
