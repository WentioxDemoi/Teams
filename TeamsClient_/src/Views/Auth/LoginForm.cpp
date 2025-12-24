#include "LoginForm.h"

LoginForm::LoginForm(QWidget* parent)
    : QWidget(parent)
{
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("Username");

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);

    loginButton = new QPushButton("Login", this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(usernameEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(loginButton);

    setLayout(layout);

    connect(loginButton, &QPushButton::clicked, this, [this]() {
    emit loginRequested(usernameEdit->text(),
                        passwordEdit->text());
});
}