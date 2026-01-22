#include "DatabaseManager.h"

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
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            email TEXT NOT NULL,
            username TEXT,
            status TEXT,
            is_me BOOLEAN DEFAULT 0,
            token TEXT,
            uuid TEXT UNIQUE
        )
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

QSqlDatabase DatabaseManager::database() const {
    return db_;
}