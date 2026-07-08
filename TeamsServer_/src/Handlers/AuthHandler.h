#ifndef AUTHHANDLER_H
#define AUTHHANDLER_H

#include "../Core/Services/AuthService.h"


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
  void handle_type(std::string payload, ResponseCallback respond);

  AuthHandler(std::shared_ptr<AuthService> authService)
      : worker_pool_(Config::instance().worker_pool_size()),
        authService_(authService) {};
  ~AuthHandler() = default;

 private:
  asio::thread_pool worker_pool_;

 protected:
  std::shared_ptr<AuthService> authService_;
};

#endif