#include "UserRepository.h"

#include <QSqlError>
#include <QSqlQuery>

#include "../Database/DatabaseManager.h"

UserRepository::UserRepository(QObject* parent)
    : QObject(parent), db_(DatabaseManager::instance().database()) {}

bool UserRepository::insert(const User& user) {
    if (isUserPresent(user.uuid())) return update(user);

    QSqlQuery query(db_);
    query.prepare(R"(
        INSERT INTO users (email, first_name, last_name, status, uuid, is_me, token, avatar, last_message)
        VALUES (:email, :firstName, :lastName, :status, :uuid, :isMe, :token, :avatar, :lastMessage)
    )");

    query.bindValue(":email",     user.email());
    query.bindValue(":firstName", user.firstName());
    query.bindValue(":lastName",  user.lastName());
    query.bindValue(":status",    user.status());
    query.bindValue(":uuid",      user.uuid());
    query.bindValue(":isMe",      isFirstUser());
    query.bindValue(":token",     user.token());
    query.bindValue(":avatar",    user.avatar());
    query.bindValue(":lastMessage", user.lastMessage());

    if (!query.exec()) {
        qDebug() << "[insert] Execution failed:" << query.lastError().text();
        return false;
    }
    return true;
}

std::optional<User> UserRepository::findByUUID(const QString& uuid) {
    QSqlQuery query(db_);
    query.prepare(R"(
        SELECT email, first_name, last_name, status, uuid, is_me, token, avatar, last_message
        FROM users
        WHERE uuid = :uuid
    )");
    query.bindValue(":uuid", uuid);

    if (!query.exec()) {
        qDebug() << "[findByUUID] Query failed:" << query.lastError().text();
        return std::nullopt;
    }

    if (!query.next()) return std::nullopt;

    User user;
    user.setEmail(query.value("email").toString());
    user.setFirstName(query.value("first_name").toString());
    user.setLastName(query.value("last_name").toString());
    user.setStatus(query.value("status").toString());
    user.setUuid(query.value("uuid").toString());
    user.setIsMe(query.value("is_me").toBool());
    user.setToken(query.value("token").toString());
    user.setAvatar(query.value("avatar").toString());
    user.setLastMessage(query.value("last_message").toString());

    return user;
}

bool UserRepository::remove(const QString& uuid) {
    QSqlQuery query(db_);
    query.prepare("DELETE FROM users WHERE uuid = :uuid");
    query.bindValue(":uuid", uuid);

    if (!query.exec()) {
        qDebug() << "[remove] Failed:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool UserRepository::update(const User& user) {
    QSqlQuery query(db_);
    query.prepare(R"(
        UPDATE users
        SET email      = :email,
            first_name = :firstName,
            last_name  = :lastName,
            status     = :status,
            token      = :token,
            avatar     = :avatar,
            last_message = :lastMessage
        WHERE uuid = :uuid
    )");

    query.bindValue(":email",     user.email());
    query.bindValue(":firstName", user.firstName());
    query.bindValue(":lastName",  user.lastName());
    query.bindValue(":status",    user.status());
    query.bindValue(":token",     user.token());
    query.bindValue(":avatar",    user.avatar());
    query.bindValue(":lastMessage", user.lastMessage());
    query.bindValue(":uuid",      user.uuid());

    if (!query.exec()) {
        qDebug() << "[update] Failed:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool UserRepository::removeAll() {
    QSqlQuery query(db_);

    if (!query.exec("DELETE FROM users")) {
        qDebug() << "[removeAll] Failed:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<User> UserRepository::findAll() {
    QList<User> users;
    QSqlQuery query(db_);

    if (!query.exec(R"(
        SELECT email, first_name, last_name, status, uuid, is_me, token, avatar, last_message
        FROM users
    )")) {
        qDebug() << "[findAll] Failed:" << query.lastError().text();
        return users;
    }

    while (query.next()) {
        User user;
        user.setEmail(query.value("email").toString());
        user.setFirstName(query.value("first_name").toString());
        user.setLastName(query.value("last_name").toString());
        user.setStatus(query.value("status").toString());
        user.setUuid(query.value("uuid").toString());
        user.setIsMe(query.value("is_me").toBool());
        user.setToken(query.value("token").toString());
        user.setAvatar(query.value("avatar").toString());
        user.setLastMessage(query.value("last_message").toString());

        users.append(user);
    }
    return users;
}

bool UserRepository::isUserPresent(const QString& uuid) {
    QSqlQuery query(db_);
    query.prepare("SELECT 1 FROM users WHERE uuid = :uuid LIMIT 1");
    query.bindValue(":uuid", uuid);

    if (!query.exec()) {
        qDebug() << "[isUserPresent] Error:" << query.lastError().text();
        return false;
    }
    return query.next();
}

bool UserRepository::isFirstUser() {
    QSqlQuery query(db_);
    if (!query.exec("SELECT COUNT(*) FROM users")) {
        qDebug() << "[isFirstUser] Error:" << query.lastError().text();
        return false;
    }

    query.next();
    return query.value(0).toInt() == 0;
}