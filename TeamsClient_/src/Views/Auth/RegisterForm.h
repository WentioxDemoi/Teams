#ifndef REGISTERFORM_H
#define REGISTERFORM_H

#include "../../includes.h"

class RegisterForm : public QWidget {
  Q_OBJECT

public:
  explicit RegisterForm(QWidget *parent = nullptr);

signals:
  void registerRequested(const QString &email, const QString &username,
                         const QString &password);

private:
  QLineEdit *usernameEdit;
  QLineEdit *emailEdit;
  QLineEdit *passwordEdit;
  QPushButton *registerButton;
};

#endif