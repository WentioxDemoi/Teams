#ifndef SESSIONSERVICE_H
#define SESSIONSERVICE_H

#include "Core/State/SessionState.h"
#include "Interfaces/IAuthService.h"
#include "Interfaces/ISessionService.h"
#include "Interfaces/ITokenManager.h"
#include "Interfaces/IUserService.h"

/**
 * @class SessionService
 * @brief Service de gestion de session utilisateur.
 *
 * Gère l'authentification et l'enregistrement des utilisateurs via un service d'authentification,
 * en fournissant des méthodes pour connecter, enregistrer et initialiser la session.
 */
class SessionService : public ISessionService {
  Q_OBJECT
 public:
  explicit SessionService(IAuthService* service = nullptr, IUserService* userService = nullptr,
                          ITokenManager* token = nullptr, QObject* parent = nullptr);
  void loginUser(const QString& username, const QString& password) override;

  void registerUser(const QString& firstName, const QString& lastName, const QString& email,
                    const QString& password) override;
  void start() override;

  public slots :
  void onLocalUserSaved(const User& user) override;
  void errorToken(const QString& error) override;

 private:
  IAuthService* authService_;
  SessionState* sessionState_;
  ITokenManager* token_;
  IUserService* userService_;
};

#endif