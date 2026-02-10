#include "AuthNetworkServiceTest.h"

void AuthNetworkServiceTest::testValidResponseEmitsAuthSuccess()
{
    FakeAuthNetworkService service;

    QSignalSpy spy(&service, &IAuthNetworkService::authSuccess);
    QByteArray json = R"({
        "success": true
    })";

    service.handleServerResponse(json);

    QCOMPARE(spy.count(), 1);
}

void AuthNetworkServiceTest::testErrorResponseEmitsAuthError()
{
    FakeAuthNetworkService service;
    QSignalSpy spy(&service, &IAuthNetworkService::authError);

    QByteArray json = R"({ "error": "Invalid credentials" })";

    service.handleServerResponse(json);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toString(),
             QString("Invalid credentials"));
}
