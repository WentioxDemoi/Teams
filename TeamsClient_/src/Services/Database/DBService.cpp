#include "DBService.h"
#include <QDir>
#include <QFile>
#include <QSqlDriver>

DBService::DBService(QObject* parent)
    : QObject(parent)
{
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
        emit dbError("Impossible d'ouvrir la base de données : " + db_.lastError().text());
        return;
    }
    
    QSqlQuery query(db_);
    QString createTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            email TEXT NOT NULL,
            username TEXT,
            status TEXT,
            is_me BOOLEAN
        )
    )";

    if (!query.exec(createTable)) {
        qDebug() << "Création table users échouée";
        emit dbError("Erreur création table users : " + query.lastError().text());
    } else {
        qDebug() << "Table users créée/vérifiée avec succès";
    }
}

void DBService::saveUser(const User& user)
{
    bool userExists = false;
    if (user.id() != 0) {
        QSqlQuery checkQuery(db_);
        checkQuery.prepare("SELECT COUNT(*) FROM users WHERE id=:id");
        checkQuery.bindValue(":id", user.id());
        if (checkQuery.exec() && checkQuery.next()) {
            userExists = (checkQuery.value(0).toInt() > 0);
            qDebug() << "Vérification existence: ID" << user.id() << "existe =" << userExists;
        }
    }
    
    QSqlQuery query(db_);

    if (user.id() == 0 || !userExists) {
        qDebug() << "Mode: INSERTION (nouvel utilisateur)";
        query.prepare("INSERT INTO users (email, username, status) "
                      "VALUES (:email, :username, :status)");
    } else {
        qDebug() << "Mode: UPDATE (utilisateur existant)";
        query.prepare("UPDATE users SET email=:email, username=:username, "
                      "status=:status WHERE id=:id");
        query.bindValue(":id", user.id());
    }

    query.bindValue(":email", user.email());
    query.bindValue(":username", user.username());
    query.bindValue(":status", user.status());

    if (!query.exec()) {
        qDebug() << "Erreur SQL:" << query.lastError().text();
        emit dbError("Erreur saveUser : " + query.lastError().text());
        return;
    }
    
    if (db_.driver()->hasFeature(QSqlDriver::Transactions)) {
        qDebug() << "Commit de la transaction...";
        db_.commit();
    }

    int newId = user.id();
    if (user.id() == 0) {
        newId = query.lastInsertId().toInt();
    }

    User savedUser = user;
    savedUser.setId(newId);
    emit userSaved(savedUser);
    showAllUsers();
}

void DBService::loadUser(int id)
{
    qDebug() << "\n=== DEBUT loadUser (id=" << id << ") ===";
    
    QSqlQuery query(db_);
    query.prepare("SELECT id, email, username, status FROM users WHERE id=:id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "❌ ERREUR loadUser:" << query.lastError().text();
        emit dbError("Erreur loadUser : " + query.lastError().text());
        return;
    }

    if (query.next()) {
        User user(
            query.value("id").toInt(),
            query.value("email").toString(),
            query.value("username").toString(),
            query.value("status").toString(),
            false
        );
        qDebug() << "✓ User trouvé:";
        user.print();
        emit userLoaded(user);
    } else {
        qDebug() << "❌ Utilisateur introuvable avec id=" << id;
        emit dbError("Utilisateur introuvable avec id=" + QString::number(id));
    }
    
    qDebug() << "=== FIN loadUser ===\n";
}

void DBService::deleteUser(int id)
{
    qDebug() << "\n=== DEBUT deleteUser (id=" << id << ") ===";
    
    QSqlQuery query(db_);
    query.prepare("DELETE FROM users WHERE id=:id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "❌ ERREUR deleteUser:" << query.lastError().text();
        emit dbError("Erreur deleteUser : " + query.lastError().text());
        return;
    }

    qDebug() << "Nombre de lignes supprimées:" << query.numRowsAffected();
    
    if (query.numRowsAffected() > 0) {
        qDebug() << "✓ User supprimé avec succès";
        emit userDeleted(id);
    } else {
        qDebug() << "❌ Utilisateur à supprimer introuvable";
        emit dbError("Utilisateur à supprimer introuvable avec id=" + QString::number(id));
    }
    
    qDebug() << "=== FIN deleteUser ===\n";
}

void DBService::showAllUsers()
{
    QSqlQuery query(db_);
    if (!query.exec("SELECT id, email, username, status FROM users")) {
        qDebug() << "❌ ERREUR showAllUsers:" << query.lastError().text();
        return;
    }

    qDebug() << "--- Contenu actuel de la DB ---";
    bool empty = true;
    int count = 0;
    while (query.next()) {
        empty = false;
        count++;
        qDebug() << "User #" << count << "{"
                 << "id:" << query.value("id").toInt()
                 << ", email:" << query.value("email").toString()
                 << ", username:" << query.value("username").toString()
                 << ", status:" << query.value("status").toString()
                 << "}";
    }
    if (empty) {
        qDebug() << "⚠️  La table users est VIDE";
    } else {
        qDebug() << "Total:" << count << "utilisateur(s)";
    }
}

void DBService::clearUsers()
{
    QSqlQuery query(db_);
    if (!query.exec("DELETE FROM users")) {
        qDebug() << "❌ ERREUR clearUsers:" << query.lastError().text();
        emit dbError("Erreur clearUsers : " + query.lastError().text());
        return;
    }
}