#ifndef CONNECTIONPOOL_H
#define CONNECTIONPOOL_H

#include "../../includes.h"

/**
 * @class ConnectionPool
 * @brief Gestion d'un pool de connexions PostgreSQL pour optimiser l'accès à la
 * base de données.
 *
 * Permet de réutiliser un nombre limité de connexions existantes au lieu d'en
 * créer de nouvelles à chaque requête, améliorant ainsi les performances et la
 * scalabilité.
 */
class ConnectionPool {
public:
  ConnectionPool(const std::string &conn_str, int size);

  std::shared_ptr<pqxx::connection> get_connection();
  void return_connection(std::shared_ptr<pqxx::connection> conn);

private:
  std::queue<std::shared_ptr<pqxx::connection>> pool_;
  std::mutex mutex_;
  std::condition_variable cv_;
};

#endif