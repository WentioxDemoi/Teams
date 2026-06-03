#ifndef USER_H
#define USER_H

#include <QDebug>
#include <QHashFunctions>
#include <QJsonObject>
#include <QObject>
#include <QString>

/**
 * @class User
 * @brief Représente un utilisateur de l'application.
 *
 * Contient les informations essentielles comme l'ID, l'email, le nom
 * d'utilisateur, le statut, et le token d'authentification (temporaire).
 */
class User {
 public:
  User() = default;
  User(const QString& email, const QString& username, const QString& status, bool isMe = false,
       const QString& token = "", const QString& uuid = "", const QString& avatar = "", const QString& lastMessage = "lastMessage");
  User(const QString& email, const QString& username, const QString& status, bool isMe = false,
       const QString& token = "", const QString& uuid = "", const QString& avatar = "");

  // Getters
  QString email() const { return email_; }
  QString username() const { return username_; }
  QString status() const { return status_; }
  bool isMe() const { return isMe_; }
  QString token() const { return token_; }
  QString uuid() const { return uuid_; }
  QString avatar() const { return avatar_; }
  QString lastMessage() const { return lastMessage_; }

  // Setters
  void setEmail(const QString& email) { email_ = email; }
  void setUsername(const QString& username) { username_ = username; }
  void setStatus(const QString& status) { status_ = status; }
  void setIsMe(bool isMe) { isMe_ = isMe; }
  void setToken(const QString& token) { token_ = token; }
  void setUuid(const QString& uuid) { uuid_ = uuid; }
  void setAvatar(const QString& avatar) { avatar_ = avatar; }
  void setLastMessage(const QString &lastMessage) {lastMessage_ = lastMessage; }

  void clearToken() { token_.clear(); }
  bool isValid() const {
    return !email_.isEmpty() && !username_.isEmpty() && !status_.isEmpty() && !uuid_.isEmpty();
  }

  // JSON conversion
  QJsonObject toJson() const;
  static User fromJson(const QJsonObject& json);

  void print() const {
    qDebug() << ", email:" << email_ << ", username:" << username_ << ", status:" << status_
             << ", token:" << token_ << ", uuid:" << uuid_ << ", isMe:" << isMe_
             << ", avatar:" << (avatar_.isEmpty() ? "false" : "true") << ", last message:" + lastMessage_
             << "}";
  }

  bool operator==(const User& other) const {
    return uuid_ == other.uuid_;
  }  // Pour le indexOf dans UserList

 private:
  QString email_;
  QString username_;
  QString status_;
  bool isMe_ = false;
  QString token_;
  QString uuid_;
  QString avatar_;
  QString lastMessage_;
};
#endif