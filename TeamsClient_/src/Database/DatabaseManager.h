#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>

/**
 * @class DatabaseManager
 * @brief Gestionnaire singleton de base de données.
 *
 * Fournit un accès global à la base de données de l'application,
 * garantissant qu'une seule instance de connexion est utilisée.
 */
class DatabaseManager {
 public:
  static DatabaseManager& instance();

  QSqlDatabase database() const;

 private:
  DatabaseManager();

  QSqlDatabase db_;
};

#endif