#include "AuthViewModel.h"

AuthViewModel::AuthViewModel(AuthService *authService, QObject* parent)
    : QObject(parent)
{
    authService_ = authService;
    connect(authService_, &AuthService::authSuccess,
            this, &AuthViewModel::loginSuccess);

    connect(authService_, &AuthService::authError,
            this, &AuthViewModel::loginError);
}

void AuthViewModel::loginUser(const QString& username, const QString& password)
{
    if (username.isEmpty() || password.isEmpty()) {
        emit loginError("Username and password required");
        return;
    }

    authService_->loginUser(username, password);
}

void AuthViewModel::registerUser(const QString& username,
                                 const QString& password)
{
    if (username.isEmpty() || password.isEmpty()) {
        emit loginError("All fields are required");
        return;
    }

    // if (!Validators::isValidEmail(email)) {
    //     emit loginError("Invalid email");
    //     return;
    // }

    authService_->registerUser(username, password);
}