#ifndef AUTHSERVICETEST_H
#define AUTHSERVICETEST_H

#include "AuthService.h"
#include "Fakes.h"

class AuthServiceTest : public QObject {
    Q_OBJECT

private slots:
    void start_withToken_callsValidate();
    void start_withoutToken_emitsNoTokenFound();
    void loginUser_delegates();
    void registerUser_delegates();
    void onUserSaved_withToken_emitsAuthSuccess();
    void onUserSaved_withoutToken_doesNotEmit();
    void errorToken_invalidToken_clearsTokenAndDeletesUser();
};

#endif