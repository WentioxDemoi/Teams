#include "LoginForm.h"

LoginForm::LoginForm(QWidget *parent) : QWidget(parent) {
  emailEdit = new QLineEdit(this);
  emailEdit->setPlaceholderText("Email");

  passwordEdit = new QLineEdit(this);
  passwordEdit->setPlaceholderText("Password");
  passwordEdit->setEchoMode(QLineEdit::Password);

  loginButton = new QPushButton("Login", this);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(emailEdit);
  layout->addWidget(passwordEdit);
  layout->addWidget(loginButton);

  setLayout(layout);

  connect(loginButton, &QPushButton::clicked, this, [this]() {
    emit loginRequested(emailEdit->text(), passwordEdit->text());
  });
}