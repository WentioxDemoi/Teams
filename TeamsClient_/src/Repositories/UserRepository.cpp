#include "UserRepository.h"

#include <QSqlError>
#include <QSqlQuery>

#include "../Database/DatabaseManager.h"

UserRepository::UserRepository(QObject *parent) : QObject(parent), db_(DatabaseManager::instance().database()) {}

bool UserRepository::save(const User &user) {
  QSqlQuery query(db_);

  query.prepare(R"(
    INSERT INTO users (
        email, first_name, last_name, status, uuid, is_me, token, avatar, last_read_at
    )
    VALUES (
        :email, :firstName, :lastName, :status, :uuid, :isMe, :token, :avatar, :lastReadAt
    )

    ON CONFLICT(uuid)
    DO UPDATE SET
        email = excluded.email,
        first_name = excluded.first_name,
        last_name = excluded.last_name,
        status = excluded.status,
        token = excluded.token,
        avatar = excluded.avatar,
        last_read_at = excluded.last_read_at
)");

  query.bindValue(":email", user.email());
  query.bindValue(":firstName", user.firstName());
  query.bindValue(":lastName", user.lastName());
  query.bindValue(":status", user.status());
  query.bindValue(":uuid", user.uuid());
  query.bindValue(":isMe", user.isMe());
  query.bindValue(":token", user.token());
  query.bindValue(":avatar", user.avatar());
  query.bindValue(":lastReadAt", user.lastReadAt().toUTC().toString(Qt::ISODate));

  if (!query.exec()) {
    qDebug() << "[save] Failed:" << query.lastError().text();
    return false;
  }

  return true;
}

std::optional<User> UserRepository::findByUUID(const QString &uuid) {
  QSqlQuery query(db_);

  query.prepare(R"(
        SELECT
            email,
            first_name,
            last_name,
            status,
            uuid,
            is_me,
            token,
            avatar,
            last_read_at
        FROM users
        WHERE uuid = :uuid
    )");

  query.bindValue(":uuid", uuid);

  if (!query.exec()) {
    qDebug() << "[findByUUID] Failed:" << query.lastError().text();
    return std::nullopt;
  }

  if (!query.next()) {
    return std::nullopt;
  }

  User user;

  user.setEmail(query.value("email").toString());
  user.setFirstName(query.value("first_name").toString());
  user.setLastName(query.value("last_name").toString());
  user.setStatus(query.value("status").toString());
  user.setUuid(query.value("uuid").toString());
  user.setIsMe(query.value("is_me").toBool());
  user.setToken(query.value("token").toString());
  user.setAvatar(query.value("avatar").toString());
  user.setLastReadAt(QDateTime::fromString(query.value("last_read_at").toString(), Qt::ISODate).toUTC());

  return user;
}

QList<User> UserRepository::findAll() {
  QList<User> users;

  QSqlQuery query(db_);

  if (!query.exec(R"(
        SELECT
            email,
            first_name,
            last_name,
            status,
            uuid,
            is_me,
            token,
            avatar,
            last_read_at
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
    user.setLastReadAt(QDateTime::fromString(query.value("last_read_at").toString(), Qt::ISODate).toUTC());

    users.append(user);
  }

  return users;
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

bool UserRepository::removeAllContacts() {
  QSqlQuery query(db_);

  if (!query.exec(R"(
        DELETE FROM users
        WHERE is_me = 0
    )")) {
    qDebug() << "[removeAllContacts] Failed:" << query.lastError().text();
    return false;
  }

  return true;
}

bool UserRepository::removeAll() {
  QSqlQuery query(db_);

  if (!query.exec("DELETE FROM users")) {
    qDebug() << "[removeAll] Failed:" << query.lastError().text();
    return false;
  }

  return true;
}

bool UserRepository::updateLastReadAt(const QString &uuid, const QString &lastReadAt) {
  QSqlQuery query(db_);

  query.prepare(R"(
      UPDATE users
      SET last_read_at = :lastReadAt
      WHERE uuid = :uuid
  )");

  query.bindValue(":lastReadAt", lastReadAt);
  query.bindValue(":uuid", uuid);

  if (!query.exec()) {
    qDebug() << "[updateLastReadAt] Failed:" << query.lastError().text();
    return false;
  }

  return query.numRowsAffected() > 0;
}