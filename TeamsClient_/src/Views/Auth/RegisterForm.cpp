#include "RegisterForm.h"

RegisterForm::RegisterForm(QWidget *parent) : QWidget(parent)
{
  usernameEdit = new QLineEdit(this);
  usernameEdit->setPlaceholderText("Username");

  emailEdit = new QLineEdit(this);
  emailEdit->setPlaceholderText("Email");

  passwordEdit = new QLineEdit(this);
  passwordEdit->setPlaceholderText("Password");
  passwordEdit->setEchoMode(QLineEdit::Password);

  registerButton = new QPushButton("Register", this);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(usernameEdit);
  layout->addWidget(emailEdit);
  layout->addWidget(passwordEdit);
  layout->addWidget(registerButton);

  setLayout(layout);

  connect(registerButton, &QPushButton::clicked, this, [this]()
          { emit registerRequested(emailEdit->text(), usernameEdit->text(),
                                   passwordEdit->text()); });
}