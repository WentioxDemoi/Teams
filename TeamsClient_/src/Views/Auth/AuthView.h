#ifndef AUTHVIEW_H
#define AUTHVIEW_H

#include "LoginForm.h"
#include "RegisterForm.h"

class AuthView : public QWidget {
  Q_OBJECT

public:
  explicit AuthView(QWidget *parent = nullptr);
  LoginForm *loginForm;
  RegisterForm *registerForm;

private slots:
  void toggleForms();
  

private:
  QPushButton *toggleButton;
  QVBoxLayout *layout;

  void updateButtonText();
};

#endif