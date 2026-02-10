#ifndef AUTHHANDLER_H
#define AUTHHANDLER_H

#include "../Core/Repositories/UserRepository.h"
#include "../Core/Services/AuthService.h"
#include "../Utils/ResponseFormater.h"
#include "../includes.h"
#include "HandlerTools.h"

using ResponseCallback = std::function<void(std::string)>;

/**
 * @class AuthHandler
 * @brief Gestionnaire des requêtes d'authentification côté serveur.
 *
 * Traite les requêtes login, inscription et validation de token reçues des
 * clients, utilise AuthService pour la logique métier et renvoie les réponses
 * via un callback.
 */
class AuthHandler {
public:
  void handle_login(std::string payload, ResponseCallback respond);
  void handle_register(std::string payload, ResponseCallback respond);
  void handle_token(std::string payload, ResponseCallback respond);

  AuthHandler(asio::thread_pool &worker_pool) : worker_pool_(worker_pool) {
    authService_ =
        std::make_unique<AuthService>(std::make_unique<UserRepository>());
  };

private:
  asio::thread_pool &worker_pool_;

protected:
  std::unique_ptr<AuthService> authService_;
};

#endif