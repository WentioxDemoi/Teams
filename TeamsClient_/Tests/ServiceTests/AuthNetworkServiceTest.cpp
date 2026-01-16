#include "AuthNetworkServiceTest.h"

void AuthNetworkServiceTest::testValidResponseEmitsAuthSuccess()
{
    AuthNetworkService service;

    QSignalSpy spy(&service, &AuthNetworkService::authSuccess);

    QByteArray json = R"({
        "data": {
            "id": 1,
            "email": "a@b.com",
            "username": "user",
            "token": "abc",
            "status": "active"
        }
    })";

    service.handleServerResponse(json);

    QCOMPARE(spy.count(), 1);
}

void AuthNetworkServiceTest::testErrorResponseEmitsAuthError()
{
    AuthNetworkService service;
    QSignalSpy spy(&service, &AuthNetworkService::authError);

    QByteArray json = R"({ "error": "Invalid credentials" })";

    service.handleServerResponse(json);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toString(),
             QString("Invalid credentials"));
}