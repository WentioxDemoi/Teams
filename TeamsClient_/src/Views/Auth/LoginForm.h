#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

/**
 * @class LoginForm
 * @brief Formulaire de connexion utilisateur.
 *
 * Fournit les champs pour l'email et le mot de passe et émet le signal
 * loginRequested lorsque l'utilisateur tente de se connecter.
 */
class LoginForm : public QWidget {
  Q_OBJECT

 public:
  explicit LoginForm(QWidget* parent = nullptr);

 signals:
  void loginRequested(const QString& email, const QString& password);

 private:
  QLineEdit* emailEdit;
  QLineEdit* passwordEdit;
  QPushButton* loginButton;
};

#endif