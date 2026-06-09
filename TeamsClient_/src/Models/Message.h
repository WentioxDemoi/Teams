#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDateTime>
#include <QString>

class Message {
 public:
  Message() = default;
Message(const QString& uuid, const QString& senderUuid, const QString& content,
        const QDateTime& timestamp, bool fromMe = false, bool isRead = false);

  QString uuid() const { return uuid_; }
  QString senderUuid() const { return senderUuid_; }
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

 private:
  QString uuid_;
  QString senderUuid_;
  QString content_;
  QDateTime timestamp_;
  bool isRead_ = false;
  bool fromMe_ = false;
};

#endif