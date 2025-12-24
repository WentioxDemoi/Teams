#ifndef LOGINFORM_H
#define LOGINFORM_H

#include "../../includes.h"

class LoginForm : public QWidget {
  Q_OBJECT

public:
  explicit LoginForm(QWidget *parent = nullptr);

signals:
  void loginRequested(const QString &username, const QString &password);

private:
  QLineEdit *usernameEdit;
  QLineEdit *passwordEdit;
  QPushButton *loginButton;
};

#endif