#ifndef LOGINFORM_H
#define LOGINFORM_H

#include "../../includes.h"

class LoginForm : public QWidget {
  Q_OBJECT

public:
  explicit LoginForm(QWidget *parent = nullptr);

signals:
  void loginRequested(const QString &email, const QString &password);

private:
  QLineEdit *emailEdit;
  QLineEdit *passwordEdit;
  QPushButton *loginButton;
};

#endif