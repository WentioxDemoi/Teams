#include "AuthView.h"

AuthView::AuthView(QWidget* parent)
    : QWidget(parent)
{
    loginForm = new LoginForm(this);
    registerForm = new RegisterForm(this);

    toggleButton = new QPushButton(this);

    layout = new QVBoxLayout(this);
    layout->addWidget(loginForm);
    layout->addWidget(registerForm);
    layout->addWidget(toggleButton);
    setLayout(layout);

    loginForm->show();
    registerForm->hide();
    updateButtonText();

    connect(toggleButton, &QPushButton::clicked, this, &AuthView::toggleForms);

    auto authVM = ServiceLocator::instance().getService<AuthViewModel>();

connect(loginForm, &LoginForm::loginRequested,
        authVM, &AuthViewModel::loginUser);

connect(registerForm, &RegisterForm::registerRequested,
        authVM, &AuthViewModel::registerUser);

connect(authVM, &AuthViewModel::loginSuccess, this,
        [](const User& user) {
            qDebug() << "Logged in as" << user.username();
        });

connect(authVM, &AuthViewModel::loginError, this,
        [](const QString& error) {
            qDebug() << "Error loggin : " << error;
        });
}

void AuthView::toggleForms()
{
    if (loginForm->isVisible()) {
        loginForm->hide();
        registerForm->show();
    } else {
        loginForm->show();
        registerForm->hide();
    }
    updateButtonText();
}

void AuthView::updateButtonText()
{
    if (loginForm->isVisible()) {
        toggleButton->setText("Switch to Register");
    } else {
        toggleButton->setText("Switch to Login");
    }
}