#ifndef SESSIONSERVICE_H
#define SESSIONSERVICE_H

#include "../Models/User.h"
#include "../includes.h"
#include "AuthService.h"
#include "Interfaces/ISessionService.h"

/**
 * @class SessionService
 * @brief Interface pour ...
 *
 * Définit les méthodes que tout service ...
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