#ifndef SESSIONSERVICETEST_H
#define SESSIONSERVICETEST_H

#include <QtTest>
#include "SessionService.h"
#include "MockAuthService.h"

class SessionServiceTest : public QObject {
    Q_OBJECT
private slots:
    void start_calls_authService();
    void loginUser_delegates();
    void registerUser_delegates();
    void relay_authSuccess();
};

#endif