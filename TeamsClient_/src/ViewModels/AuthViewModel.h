#ifndef AUTHVIEWMODEL_H
#define AUTHVIEWMODEL_H

#include "../Models/User.h"
#include "../Services/Interfaces/IAuthService.h"

class UserState;
class SessionState;

/**
 * @class AuthViewModel
 * @brief ViewModel pour la gestion de l'authentification.
 *
 * Sert de couche intermédiaire entre l'interface utilisateur et le service d'authentification.
 * Gère la logique métier liée au login, l'inscription et l'état de session.
 */
class AuthViewModel : public QObject {
  Q_OBJECT

 public:
  explicit AuthViewModel(IAuthService* authService = nullptr,
                         UserState* userState = nullptr,
                         SessionState* sessionState = nullptr,
                         QObject* parent = nullptr);

 public slots:
  void loginUser(const QString& email, const QString& password);
  void registerUser(const QString& firstName, const QString& lastName, const QString& email, const QString& password);
  void start();

 signals:
  void authSuccess(const User& user);
  void authError(const QString& error);
  void noTokenFound();
  void registerWithServer4WebRTC(QString UUID);

 private:
  IAuthService* authService_;
  UserState* userState_;
  SessionState* sessionState_;
};

#endif