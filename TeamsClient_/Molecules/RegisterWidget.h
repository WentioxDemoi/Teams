#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include "../Atoms/InputField.h"
#include "../Atoms/Button.h"

class Auth;

class RegisterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWidget(Auth *parent = nullptr);
    bool init();

signals:
    void registerRequest(const QString &email, const QString &password, bool isLogin);

private:
    void onRegisterButtonClicked();

    QVBoxLayout *layout;
    InputField *emailField;
    InputField *passwordField;
    InputField *confirmPasswordField;
    Button *registerButton;

    Auth *authParent;
};

#endif