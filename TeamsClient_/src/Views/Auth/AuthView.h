#ifndef AUTHVIEW_H
#define AUTHVIEW_H

#include "../../ViewModels/AuthViewModel.h"
#include "../../Core/ServiceLocator.h"
#include "LoginForm.h"
#include "RegisterForm.h"

class AuthView : public QWidget
{
    Q_OBJECT

public:
    explicit AuthView(QWidget* parent = nullptr);

private slots:
    void toggleForms();

private:
    LoginForm* loginForm;
    RegisterForm* registerForm;
    QPushButton* toggleButton;
    QVBoxLayout* layout;

    void updateButtonText();
};

#endif // AUTHVIEW_H