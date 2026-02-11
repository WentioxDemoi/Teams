#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "../includes.h"

class DatabaseManager {
public:
  static DatabaseManager &instance();

  QSqlDatabase database() const;

private:
  DatabaseManager();

  QSqlDatabase db_;
};

#endif