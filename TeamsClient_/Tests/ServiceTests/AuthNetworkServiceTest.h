#ifndef AUTHNETWORKSERVICETEST_H
#define AUTHNETWORKSERVICETEST_H

#include <QtTest>
#include <QSignalSpy>
#include "AuthNetworkService.h"

class AuthNetworkServiceTest : public QObject {
    Q_OBJECT
private slots:
    void testValidResponseEmitsAuthSuccess();
    void testErrorResponseEmitsAuthError();
};

#endif