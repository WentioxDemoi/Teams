#include "AuthView.h"

AuthView::AuthView(QWidget *parent) : QWidget(parent) {
  authViewModel = new AuthViewModel();
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

  connect(loginForm, &LoginForm::loginRequested, authViewModel,
          &AuthViewModel::loginUser);

  connect(registerForm, &RegisterForm::registerRequested, authViewModel,
          &AuthViewModel::registerUser);

  connect(
      authViewModel, &AuthViewModel::loginSuccess, this,
      [](const User &user) { qDebug() << "Logged in as" << user.username(); });

  connect(authViewModel, &AuthViewModel::loginError, this,
          [](const QString &error) { qDebug() << "Error loggin : " << error; });
}

void AuthView::toggleForms() {
  if (loginForm->isVisible()) {
    loginForm->hide();
    registerForm->show();
  } else {
    loginForm->show();
    registerForm->hide();
  }
  updateButtonText();
}

void AuthView::updateButtonText() {
  if (loginForm->isVisible()) {
    toggleButton->setText("Switch to Register");
  } else {
    toggleButton->setText("Switch to Login");
  }
}