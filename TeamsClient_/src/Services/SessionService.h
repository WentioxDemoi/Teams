#ifndef SESSIONSERVICE_H
#define SESSIONSERVICE_H

#include "Interfaces/IAuthService.h"
#include "Interfaces/ISessionService.h"

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
  explicit SessionService(IAuthService *service = nullptr, QObject *parent = nullptr);
  void loginUser(const QString &username, const QString &password) override;

  void registerUser(const QString &email, const QString &username,
                    const QString &password) override;
  void start() override;

private:
    IAuthService *authService_;
};

#endif