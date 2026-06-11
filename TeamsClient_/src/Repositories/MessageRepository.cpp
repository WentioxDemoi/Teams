#include "MessageRepository.h"

#include <QSqlError>
#include <QSqlQuery>

#include "../Database/DatabaseManager.h"

MessageRepository::MessageRepository(QObject* parent)
    : QObject(parent), db_(DatabaseManager::instance().database()) {}

std::optional<Message> MessageRepository::findByUUID(const QString& uuid) {
  QSqlQuery query(db_);
  query.prepare(R"(
      SELECT uuid, sender_uuid, receiver_uuid, type, content, timestamp, is_read, from_me
      FROM messages
      WHERE uuid = :uuid
  )");
  query.bindValue(":uuid", uuid);

  if (!query.exec()) {
    qDebug() << "[findByUUID] Query failed:" << query.lastError().text();
    return std::nullopt;
  }

  if (!query.next()) return std::nullopt;

  Message message(
      query.value("uuid").toString(),
      query.value("sender_uuid").toString(),
      query.value("receiver_uuid").toString(),
      query.value("type").toString(),
      query.value("content").toString(),
      QDateTime::fromString(query.value("timestamp").toString(), Qt::ISODate),
      query.value("from_me").toBool(),
      query.value("is_read").toBool());

  return message;
}


QList<Message> MessageRepository::findAll() {
  QList<Message> messages;
  QSqlQuery query(db_);

  if (!query.exec(R"(
      SELECT uuid, sender_uuid, receiver_uuid, type, content, timestamp, is_read, from_me
      FROM messages
      ORDER BY timestamp ASC
  )")) {
    qDebug() << "[findAll] Failed:" << query.lastError().text();
    return messages;
  }

  while (query.next()) {
    messages.append(Message(
        query.value("uuid").toString(),
        query.value("sender_uuid").toString(),
        query.value("receiver_uuid").toString(),
        query.value("type").toString(),
        query.value("content").toString(),
        QDateTime::fromString(query.value("timestamp").toString(), Qt::ISODate),
        query.value("from_me").toBool(),
        query.value("is_read").toBool()));
  }

  return messages;
}

QList<Message> MessageRepository::findForConversation(const QString& userUuid1, const QString& userUuid2) {
  QList<Message> messages;
  QSqlQuery query(db_);
  query.prepare(R"(
      SELECT uuid, sender_uuid, receiver_uuid, type, content, timestamp, is_read, from_me
      FROM messages
      WHERE (sender_uuid = :user1 AND receiver_uuid = :user2)
         OR (sender_uuid = :user2 AND receiver_uuid = :user1)
      ORDER BY timestamp ASC
  )");
  query.bindValue(":user1", userUuid1);
  query.bindValue(":user2", userUuid2);

  if (!query.exec()) {
    qDebug() << "[findForConversation] Failed:" << query.lastError().text();
    return messages;
  }

  while (query.next()) {
    messages.append(Message(
        query.value("uuid").toString(),
        query.value("sender_uuid").toString(),
        query.value("receiver_uuid").toString(),
        query.value("type").toString(),
        query.value("content").toString(),
        QDateTime::fromString(query.value("timestamp").toString(), Qt::ISODate),
        query.value("from_me").toBool(),
        query.value("is_read").toBool()));
  }

  return messages;
}

QList<Message> MessageRepository::findForParticipant(const QString& participantUuid) {
  QList<Message> messages;
  QSqlQuery query(db_);
  query.prepare(R"(
      SELECT uuid, sender_uuid, receiver_uuid, type, content, timestamp, is_read, from_me
      FROM messages
      WHERE sender_uuid = :participant OR receiver_uuid = :participant
      ORDER BY timestamp ASC
  )");
  query.bindValue(":participant", participantUuid);

  if (!query.exec()) {
    qDebug() << "[findForParticipant] Failed:" << query.lastError().text();
    return messages;
  }

  while (query.next()) {
    messages.append(Message(
        query.value("uuid").toString(),
        query.value("sender_uuid").toString(),
        query.value("receiver_uuid").toString(),
        query.value("type").toString(),
        query.value("content").toString(),
        QDateTime::fromString(query.value("timestamp").toString(), Qt::ISODate),
        query.value("from_me").toBool(),
        query.value("is_read").toBool()));
  }

  return messages;
}

bool MessageRepository::save(const Message& message) {
    QSqlQuery query(db_);

    query.prepare(R"(
        INSERT INTO messages (
            uuid,
            sender_uuid,
            receiver_uuid,
            type,
            content,
            timestamp,
            is_read,
            from_me
        )
        VALUES (
            :uuid,
            :senderUuid,
            :receiverUuid,
            :type,
            :content,
            :timestamp,
            :isRead,
            :fromMe
        )

        ON CONFLICT(uuid)
        DO UPDATE SET
            sender_uuid = excluded.sender_uuid,
            receiver_uuid = excluded.receiver_uuid,
            type = excluded.type,
            content = excluded.content,
            timestamp = excluded.timestamp,
            is_read = excluded.is_read,
            from_me = excluded.from_me
    )");

    query.bindValue(":uuid", message.uuid());
    query.bindValue(":senderUuid", message.senderUuid());
    query.bindValue(":receiverUuid", message.receiverUuid());
    query.bindValue(":type", message.type());
    query.bindValue(":content", message.content());
    query.bindValue(":timestamp", message.timestamp().toString(Qt::ISODate));
    query.bindValue(":isRead", message.isRead());
    query.bindValue(":fromMe", message.fromMe());

    if (!query.exec()) {
        qDebug() << "[save] Failed:" << query.lastError().text();
        return false;
    }

    return true;
}

bool MessageRepository::remove(const QString& uuid) {
  QSqlQuery query(db_);
  query.prepare("DELETE FROM messages WHERE uuid = :uuid");
  query.bindValue(":uuid", uuid);

  if (!query.exec()) {
    qDebug() << "[remove] Failed:" << query.lastError().text();
    return false;
  }
  return query.numRowsAffected() > 0;
}

bool MessageRepository::removeAll() {
  QSqlQuery query(db_);
  if (!query.exec("DELETE FROM messages")) {
    qDebug() << "[removeAll] Failed:" << query.lastError().text();
    return false;
  }
  return true;
}
