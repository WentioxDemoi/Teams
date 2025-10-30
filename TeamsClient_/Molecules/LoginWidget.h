#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include "../Atoms/InputField.h"
#include "../Atoms/Button.h"

class Auth;

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(Auth *parent = nullptr);
    bool init();

signals:
    void loginRequest(const QString &email, const QString &password, bool isLogin);

private:
    void onLoginButtonClicked();

    QVBoxLayout *layout;
    InputField *emailField;
    InputField *passwordField;
    Button *loginButton;

    Auth *authParent;
};

#endif