#include "DatabaseManager.h"

#include <QSqlQuery>

DatabaseManager::DatabaseManager() {
  if (!QSqlDatabase::contains("AppConnection")) {
    qDebug() << "Création de la connexion AppConnection";
    db_ = QSqlDatabase::addDatabase("QSQLITE", "AppConnection");
    db_.setDatabaseName("app_database.db");
  } else {
    qDebug() << "Connexion AppConnection existe déjà";
    db_ = QSqlDatabase::database("AppConnection");
  }

  if (!db_.open()) {
    qDebug() << "Impossible d'ouvrir la base de données";
    return;
  }

  QSqlQuery query(db_);
  QString createTable = R"(
        CREATE TABLE IF NOT EXISTS users (
    email      TEXT NOT NULL,
    first_name TEXT NOT NULL,
    last_name  TEXT NOT NULL,
    status     TEXT,
    uuid       TEXT PRIMARY KEY,
    is_me      INTEGER DEFAULT 0,
    token      TEXT,
    avatar     TEXT
);
    )";

  if (!query.exec(createTable)) {
    qDebug() << "Création table users échouée";
  } else {
    qDebug() << "Table users créée/vérifiée avec succès";
  }
}

DatabaseManager& DatabaseManager::instance() {
  static DatabaseManager instance;
  return instance;
}

QSqlDatabase DatabaseManager::database() const { return db_; }