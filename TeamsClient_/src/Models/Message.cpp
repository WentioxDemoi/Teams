#include "Message.h"

#include <QJsonObject>
#include "Core/State/UserState.h"

Message::Message(const QString& uuid, const QString& senderUuid, const QString& receiverUuid,
                 const QString& chatType, const QString& content, const QDateTime& timestamp)
    : uuid_(uuid.isEmpty() ? generateUuid() : uuid),
      senderUuid_(!senderUuid.isEmpty() ? senderUuid : UserState::instance().localUser().uuid()),
      receiverUuid_(receiverUuid),
      chatType_(chatType),
      content_(content),
      timestamp_(timestamp),
      fromMe_(UserState::instance().localUser().uuid() == senderUuid) {}

Message Message::fromJson(const QJsonObject& json) {
  return Message(
      json["uuid"].toString(),
      json["senderUuid"].toString(),
      json["receiverUuid"].toString(),
      json["chatType"].toString(),
      json["content"].toString(),
      QDateTime::fromString(json["timestamp"].toString(), Qt::ISODate));
}

QJsonObject Message::toJson() const {
  return {
      {"uuid", uuid_},
      {"senderUuid", senderUuid_},
      {"receiverUuid", receiverUuid_},
      {"chatType", chatType_},
      {"content", content_},
      {"timestamp", timestamp_.toString(Qt::ISODate)}
  };
}

Message Message::createOutgoing(const QString& receiverUuid, const QString& chatType,
                                  const QString& content) {
  return Message(generateUuid(), UserState::instance().localUser().uuid(), receiverUuid,
                 chatType, content, QDateTime::currentDateTime());
}

// Génération d'un UUID qui devra être temporaire pour ensuite être remplacé par le UUID V7 créé par le serveur
// L'UUID V7 nous servira à organiser les messages car ils contienent un élément temporel.
// Actuellement pour organiser les messages, on se base sur la date de création du message créée par le client lui même. Ce n'est pas bon, le serveur doit être la source de vérité.
QString Message::generateUuid() {
  QString id = QUuid::createUuid().toString();
  id.remove('{');
  id.remove('}');
  return id;
}

bool Message::isValid() const {
    return !uuid_.isEmpty() &&
           !senderUuid_.isEmpty() &&
           !receiverUuid_.isEmpty() &&
           !chatType_.isEmpty() &&
           !content_.isEmpty() &&
           timestamp_.isValid();
}
