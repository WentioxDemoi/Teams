#include "AuthView.h"

AuthView::AuthView(QWidget *parent) : QWidget(parent) {
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