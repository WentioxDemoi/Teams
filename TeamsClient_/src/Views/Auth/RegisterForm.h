#ifndef REGISTERFORM_H
#define REGISTERFORM_H
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

/**
 * @class RegisterForm
 * @brief Formulaire d'inscription utilisateur.
 *
 * Fournit les champs pour l'email, le nom d'utilisateur et le mot de passe,
 * et émet le signal registerRequested lorsque l'utilisateur tente de s'inscrire.
 */
class RegisterForm : public QWidget {
  Q_OBJECT

 public:
  explicit RegisterForm(QWidget* parent = nullptr);

 signals:
  void registerRequested(const QString& email, const QString& username, const QString& password);

 private:
  QLineEdit* usernameEdit;
  QLineEdit* emailEdit;
  QLineEdit* passwordEdit;
  QPushButton* registerButton;
};

#endif