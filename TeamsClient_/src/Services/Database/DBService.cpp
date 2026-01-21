#include "DBService.h"
#include <QSqlDriver>
#include <QRandomGenerator>
#include <QDebug>
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
            is_me BOOLEAN DEFAULT 0,
            token TEXT,
            uuid TEXT UNIQUE
        )
    )";

    if (!query.exec(createTable)) {
        qDebug() << "Création table users échouée";
        emit dbError("Erreur création table users : " + query.lastError().text());
    } else {
        qDebug() << "Table users créée/vérifiée avec succès";
    }
}


void DBService::saveUser(const User &user) {
    if (!db_.isOpen()) {
        emit dbError("Base de données non ouverte !");
        qDebug() << "[saveUser] DB not open!";
        return;
    }

    // 1️⃣ Vérifier si l'utilisateur existe via UUID
    QSqlQuery checkQuery(db_);
    checkQuery.prepare("SELECT id FROM users WHERE uuid=:uuid");
    checkQuery.bindValue(":uuid", user.uuid());
    
    int existingId = 0;
    if (checkQuery.exec() && checkQuery.next()) {
        existingId = checkQuery.value(0).toInt();
        qDebug() << "[saveUser] User exists with id=" << existingId;
    } else if (checkQuery.lastError().isValid()) {
        emit dbError("Erreur vérification existence user : " + checkQuery.lastError().text());
        qDebug() << "[saveUser] Error checking user existence:" << checkQuery.lastError().text();
        return;
    } else {
        qDebug() << "[saveUser] User does not exist, will insert";
    }

    // 2️⃣ Déterminer si c'est le premier utilisateur pour isMe
    bool isFirstUser = false;
    if (existingId == 0) {
        QSqlQuery countQuery(db_);
        if (countQuery.exec("SELECT COUNT(*) FROM users") && countQuery.next()) {
            int userCount = countQuery.value(0).toInt();
            isFirstUser = (userCount == 0);
            qDebug() << "[saveUser] Total users in DB =" << userCount
                     << "| isFirstUser =" << isFirstUser;
        } else if (countQuery.lastError().isValid()) {
            emit dbError("Erreur comptage utilisateurs : " + countQuery.lastError().text());
            qDebug() << "[saveUser] Error counting users:" << countQuery.lastError().text();
            return;
        }
    }

    // 3️⃣ Préparer INSERT ou UPDATE
    QSqlQuery query(db_);
    if (existingId == 0) {
        // INSERT
        query.prepare(R"(
            INSERT INTO users (email, username, status, uuid, is_me, token)
            VALUES (:email, :username, :status, :uuid, :isMe, :token)
        )");
        query.bindValue(":email", user.email());
        query.bindValue(":username", user.username());
        query.bindValue(":status", user.status());
        query.bindValue(":uuid", user.uuid());
        query.bindValue(":isMe", isFirstUser ? 1 : 0);
        query.bindValue(":token", user.token());
        qDebug() << "[saveUser] Preparing INSERT | isMe =" << (isFirstUser ? 1 : 0);
    } else {
        // UPDATE
        query.prepare(R"(
            UPDATE users
            SET email=:email, username=:username, status=:status, token=:token
            WHERE id=:id
        )");
        query.bindValue(":id", existingId);
        query.bindValue(":email", user.email());
        query.bindValue(":username", user.username());
        query.bindValue(":status", user.status());
        query.bindValue(":token", user.token());
        qDebug() << "[saveUser] Preparing UPDATE for id =" << existingId;
    }

    qDebug() << "[saveUser] Binding values | email:" << user.email()
             << "| username:" << user.username()
             << "| status:" << user.status()
             << "| uuid:" << user.uuid();

    // 4️⃣ Exécuter la requête
    if (!query.exec()) {
        emit dbError("Erreur saveUser : " + query.lastError().text());
        qDebug() << "[saveUser] Execution failed:" << query.lastError().text();
        return;
    }
    qDebug() << "[saveUser] Query executed successfully";

    // 5️⃣ Récupérer l'ID réel
    int newId = existingId == 0 ? query.lastInsertId().toInt() : existingId;
    User tmp = user;
    tmp.setId(newId);
    if (existingId == 0) tmp.setIsMe(isFirstUser);

    qDebug() << "[saveUser] User saved | id:" << newId << "| isMe:" << tmp.isMe();

    emit userSaved(tmp);
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



