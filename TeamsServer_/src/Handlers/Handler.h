#ifndef HANDLER_H
#define HANDLER_H

#include "../Utils/Config.h"
#include "AuthHandler.h"
#include "HandlerTools.h"

/**
 * @class Handler
 * @brief Point d'entrée central pour le traitement des requêtes côté serveur.
 *
 * Délègue le traitement des différents types de requêtes (actuellement
 * authentification) aux gestionnaires spécialisés tels que AuthHandler.
 */
class Handler {
public:
  void handle_type(std::string payload, ResponseCallback respond);
  Handler();

private:
  std::unique_ptr<AuthHandler> authHandler_;
  asio::thread_pool worker_pool_;
};

#endif