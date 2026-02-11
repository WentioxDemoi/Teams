#ifndef AUTHVIEWMODEL_H
#define AUTHVIEWMODEL_H

#include "../Core/ServiceLocator.h"
#include "../Models/User.h"
#include "../includes.h"
#include "ViewModelsTools.h"
#include "../Services/SessionService.h"
#include "../Services/Interfaces/ISessionService.h"

/**
 * @class AuthViewModel
 * @brief ViewModel pour la gestion de l'authentification.
 *
 * Sert de couche intermédiaire entre l'interface utilisateur et le service d'authentification.
 * Gère la logique métier liée au login et à l'inscription, émet des signaux
 * authSuccess ou loginError selon le résultat, et facilite les tests unitaires via IAuthService.
 */
class AuthViewModel : public QObject
{
  Q_OBJECT

public:
  explicit AuthViewModel(ISessionService *service = nullptr, QObject *parent = nullptr); // Aller voir dans le CPP l'implémentation
  void start();

public slots:
  void loginUser(const QString &email, const QString &password);
  void registerUser(const QString &email, const QString &username,
                    const QString &password);
signals:
  void authSuccess(const User &user);
  void authError(const QString &error);
  void noTokenFound();

private:

  ISessionService *sessionService_; // On passe par une interface pour faciliter les tests unitaires
};

#endif