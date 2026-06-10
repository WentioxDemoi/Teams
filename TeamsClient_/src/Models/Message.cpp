#include "Message.h"

#include <QJsonObject>

Message::Message(const QString& uuid, const QString& senderUuid, const QString& receiverUuid,
                 const QString& type, const QString& content, const QDateTime& timestamp,
                 bool fromMe, bool isRead)
    : uuid_(uuid),
      senderUuid_(senderUuid),
      receiverUuid_(receiverUuid),
      type_(type),
      content_(content),
      timestamp_(timestamp),
      fromMe_(fromMe),
      isRead_(isRead) {}

Message Message::fromJson(const QJsonObject& json) {
  return Message(
      json["uuid"].toString(),
      json["senderUuid"].toString(),
      json["receiverUuid"].toString(),
      json["type"].toString(),
      json["content"].toString(),
      QDateTime::fromString(json["timestamp"].toString(), Qt::ISODate),
      json["fromMe"].toBool(),
      json["isRead"].toBool());
}

QJsonObject Message::toJson() const {
  return {
      {"uuid", uuid_},
      {"senderUuid", senderUuid_},
      {"receiverUuid", receiverUuid_},
      {"type", type_},
      {"content", content_},
      {"timestamp", timestamp_.toString(Qt::ISODate)},
      {"fromMe", fromMe_},
      {"isRead", isRead_},
  };
}