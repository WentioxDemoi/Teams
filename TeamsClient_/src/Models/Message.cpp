#include "Message.h"

#include <QJsonObject>
#include "Core/State/UserState.h"

Message::Message(const QString& uuid, const QString& senderUuid, const QString& receiverUuid,
                 const QString& chatType, const QString& content, const QDateTime& timestamp,
                 bool fromMe, bool isRead)
    : uuid_(uuid.isEmpty() ? generateUuid() : uuid),
      senderUuid_(!senderUuid.isEmpty() ? senderUuid : UserState::instance().localUser().uuid()),
      receiverUuid_(receiverUuid),
      chatType_(chatType),
      content_(content),
      timestamp_(timestamp),
      fromMe_(fromMe),
      isRead_(isRead) {}

Message Message::fromJson(const QJsonObject& json) {
  return Message(
      json["uuid"].toString(),
      json["senderUuid"].toString(),
      json["receiverUuid"].toString(),
      json["chatType"].toString(),
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
      {"chatType", chatType_},
      {"content", content_},
      {"timestamp", timestamp_.toString(Qt::ISODate)},
      {"fromMe", fromMe_},
      {"isRead", isRead_},
  };
}

Message Message::createOutgoing(const QString& receiverUuid, const QString& chatType,
                                  const QString& content) {
  return Message(generateUuid(), UserState::instance().localUser().uuid(), receiverUuid,
                 chatType, content, QDateTime::currentDateTime(), true, false);
}

QString Message::generateUuid() {
  QString id = QUuid::createUuid().toString();
  id.remove('{');
  id.remove('}');
  return id;
}
