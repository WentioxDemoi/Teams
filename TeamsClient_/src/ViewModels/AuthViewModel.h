#ifndef AUTHVIEWMODEL_H
#define AUTHVIEWMODEL_H

#include "../Models/User.h"
#include "../Services/AuthService.h"
#include "../includes.h"
#include "../Core/ServiceLocator.h"

class AuthService;

class AuthViewModel : public QObject {
  Q_OBJECT

public:
  explicit AuthViewModel(QObject *parent = nullptr);

public slots:
  void loginUser(const QString &email, const QString &password);
  void registerUser(const QString &email, const QString &username, const QString &password);

signals:
  void loginSuccess(const User &user);
  void loginError(const QString &error);

private:
  AuthService *authService_;
};

#endif