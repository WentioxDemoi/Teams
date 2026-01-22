#include "UserRepository.h"

UserRepository::UserRepository(QObject *parent)
    : QObject(parent), db_(DatabaseManager::instance().database()) {}

bool UserRepository::insert(const User &user) {
  if (isUserPresent(user.uuid()))
    return update(user);

  QSqlQuery query(db_);
  query.prepare(R"(
        INSERT INTO users (email, username, status, uuid, is_me, token)
        VALUES (:email, :username, :status, :uuid, :isMe, :token)
    )");

  query.bindValue(":email", user.email());
  query.bindValue(":username", user.username());
  query.bindValue(":status", user.status());
  query.bindValue(":uuid", user.uuid());
  query.bindValue(":isMe", isFirstUser());
  query.bindValue(":token", user.token());

  if (!query.exec()) {
    qDebug() << "[insert] Execution failed:" << query.lastError().text();
    return false;
  }
  return true;
}

std::optional<User> UserRepository::findByUUID(const QString &uuid) {
  QSqlQuery query(db_);
  query.prepare(R"(
        SELECT email, username, status, uuid, is_me, token
        FROM users
        WHERE uuid = :uuid
    )");
  query.bindValue(":uuid", uuid);

  if (!query.exec()) {
    qDebug() << "[findByUUID] Query failed:" << query.lastError().text();
    return std::nullopt;
  }

  if (!query.next())
    return std::nullopt;

  User user;
  user.setEmail(query.value("email").toString());
  user.setUsername(query.value("username").toString());
  user.setStatus(query.value("status").toString());
  user.setUuid(query.value("uuid").toString());
  user.setIsMe(query.value("is_me").toBool());
  user.setToken(query.value("token").toString());

  return user;
}

bool UserRepository::remove(const QString &uuid) {
  QSqlQuery query(db_);
  query.prepare("DELETE FROM users WHERE uuid = :uuid");
  query.bindValue(":uuid", uuid);

  if (!query.exec()) {
    qDebug() << "[remove] Failed:" << query.lastError().text();
    return false;
  }
  return query.numRowsAffected() > 0;
}

bool UserRepository::update(const User &user) {
  QSqlQuery query(db_);
  query.prepare(R"(
        UPDATE users
        SET email = :email,
            username = :username,
            status = :status,
            token = :token
        WHERE uuid = :uuid
    )");

  query.bindValue(":email", user.email());
  query.bindValue(":username", user.username());
  query.bindValue(":status", user.status());
  query.bindValue(":token", user.token());
  query.bindValue(":uuid", user.uuid());

  if (!query.exec()) {
    qDebug() << "[update] Failed:" << query.lastError().text();
    return false;
  }
  return query.numRowsAffected() > 0;
}

QList<User> UserRepository::findAll() {
  QList<User> users;
  QSqlQuery query(db_);

  if (!query.exec(R"(
        SELECT email, username, status, uuid, is_me, token
        FROM users
    )")) {
    qDebug() << "[findAll] Failed:" << query.lastError().text();
    return users;
  }

  while (query.next()) {
    User user;
    user.setEmail(query.value("email").toString());
    user.setUsername(query.value("username").toString());
    user.setStatus(query.value("status").toString());
    user.setUuid(query.value("uuid").toString());
    user.setIsMe(query.value("is_me").toBool());
    user.setToken(query.value("token").toString());

    users.append(user);
  }
  return users;
}

bool UserRepository::isUserPresent(const QString &uuid) {
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
  int count = query.value(0).toInt();
  return count == 0;
}