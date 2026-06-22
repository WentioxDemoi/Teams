#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDateTime>
#include <QString>

#include <QJsonObject>
#include <QtCore/qdatetime.h>

/**
 * @class Message
 * @brief Représente un message échangé entre utilisateurs.
 *
 * Contient les métadonnées d'un message, son contenu, son auteur, sa date,
 * ainsi que des indicateurs de lecture et d'origine.
 */
class Message {
 public:
    Message() = default;
    Message(const QString& uuid, const QString& senderUuid, const QString& receiverUuid,
        const QString& chatType, const QString& content, const QDateTime& timestamp,
        bool fromMe = false, bool isRead = false);

  QString uuid() const { return uuid_; }
  QString senderUuid() const { return senderUuid_; }
  QString receiverUuid() const { return receiverUuid_; }
  QString chatType() const { return chatType_; }
  QString content() const { return content_; }
  QDateTime timestamp() const { return timestamp_; }
  bool isRead() const { return isRead_; }
  bool fromMe() const { return fromMe_; }
  
  void setFromMe(bool fromMe) { fromMe_ = fromMe; }
  void setIsRead(bool isRead) { isRead_ = isRead; }
  void setContent(const QString& content) { content_ = content; }

  bool operator==(const Message& other) const { return uuid_ == other.uuid_; }

  static Message fromJson(const QJsonObject& json);
  QJsonObject toJson() const;

  void setUuid(const QString& uuid) { uuid_ = uuid; }
  void setSenderUuid(const QString& senderUuid) { senderUuid_ = senderUuid; }
  void setReceiverUuid(const QString& receiverUuid) { receiverUuid_ = receiverUuid; }
  void setChatType(const QString& chatType) { chatType_ = chatType; }
  void setTimestamp(const QDateTime& timestamp) { timestamp_ = timestamp; }

  static Message createOutgoing(const QString& receiverUuid, const QString& chatType,
                                  const QString& content);

  static QString generateUuid();

 private:
  QString uuid_;
  QString senderUuid_;
  QString receiverUuid_;
  QString chatType_ = "message";
  QString content_;
  QDateTime timestamp_;
  bool isRead_ = false;
  bool fromMe_ = false;
};

#endif