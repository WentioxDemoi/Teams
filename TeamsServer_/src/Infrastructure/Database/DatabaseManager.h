// infrastructure/database/DatabaseManager.h (simplifié)
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "../../Utils/Config.h"
#include "ConnectionPool.h"

/**
 * @class DatabaseManager
 * @brief Gestionnaire centralisé des connexions à la base de données.
 *
 * Fournit un accès singleton aux connexions PostgreSQL via ConnectionPool,
 * encapsulant la création, la libération et l'initialisation de la base.
 */
class DatabaseManager {
public:
  static DatabaseManager &instance();

  std::shared_ptr<pqxx::connection> acquire_connection();
  void release_connection(std::shared_ptr<pqxx::connection> conn);

private:
  DatabaseManager();
  ~DatabaseManager() = default;

  DatabaseManager(const DatabaseManager &) = delete;
  DatabaseManager(DatabaseManager &&) = delete;
  DatabaseManager &operator=(const DatabaseManager &) = delete;
  DatabaseManager &operator=(DatabaseManager &&) = delete;

  void initialize();
  void initialize_schema();

  std::unique_ptr<ConnectionPool> pool_;
  bool initialized_ = false;
  Config &config_;
};

#endif