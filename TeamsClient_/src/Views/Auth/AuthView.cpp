#include "AuthView.h"

AuthView::AuthView(QWidget *parent) : QWidget(parent)
{
  loginForm = new LoginForm(this);
  registerForm = new RegisterForm(this);

  toggleButton = new QPushButton(this);

  stack = new QStackedWidget(this);
  stack->addWidget(loginForm);
  stack->addWidget(registerForm);

  layout = new QVBoxLayout(this);
  layout->addStretch();
  layout->addWidget(stack, 0, Qt::AlignCenter);
  layout->addWidget(toggleButton, 0, Qt::AlignCenter);
  layout->addStretch();

  stack->setCurrentWidget(loginForm);
  updateButtonText();

  connect(toggleButton, &QPushButton::clicked, this, &AuthView::toggleForms);
}

void AuthView::toggleForms()
{
  if (stack->currentWidget() == loginForm)
    stack->setCurrentWidget(registerForm);
  else
    stack->setCurrentWidget(loginForm);
  updateButtonText();
}

void AuthView::updateButtonText()
{
  if (stack->currentWidget() == loginForm)
  {
    toggleButton->setText("Switch to Register");
  }
  else
  {
    toggleButton->setText("Switch to Login");
  }
}