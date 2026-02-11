#ifndef AUTHVIEW_H
#define AUTHVIEW_H

#include "LoginForm.h"
#include "RegisterForm.h"

/**
 * @class AuthView
 * @brief Interface graphique pour l'authentification.
 *
 * Contient et gère les formulaires de login et d'inscription, ainsi qu'un
 * bouton pour basculer entre les deux. Coordonne l'affichage et l'interaction
 * avec l'utilisateur.
 */
class AuthView : public QWidget
{
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
  QStackedWidget *stack;

  void updateButtonText();
};

#endif