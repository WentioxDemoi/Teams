#ifndef USER_H
#define USER_H

#include <QDebug>
#include <QHashFunctions>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QtCore/qtmetamacros.h>
#include <qqmlintegration.h>

/**
 * @class User
 * @brief Représente un utilisateur de l'application.
 *
 * Expose les informations d'un utilisateur (email, nom, statut, avatar, etc.)
 * et fournit la sérialisation JSON pour le transfert réseau ou le stockage.
 */
class User {
  Q_GADGET
  QML_VALUE_TYPE(user)

  Q_PROPERTY(QString uuid READ uuid CONSTANT)
  Q_PROPERTY(QString username READ fullName CONSTANT)
  Q_PROPERTY(QString initials READ initials CONSTANT)
  Q_PROPERTY(QString avatarColor READ avatarColor CONSTANT)
  Q_PROPERTY(bool online READ online CONSTANT)
  Q_PROPERTY(QString lastMessage READ lastMessage CONSTANT)
  Q_PROPERTY(QString status READ status CONSTANT)
  Q_PROPERTY(int unreadCount READ unreadCount WRITE setUnreadCount NOTIFY unreadCountChanged)

public:
  User() = default;
  User(const QString &email, const QString &firstName, const QString &lastName, const QString &status,
       bool isMe = false, const QString &token = "", const QString &uuid = "", const QString &avatar = "",
       const QString &lastMessage = "");

  // Getters
  QString email() const { return email_; }
  QString firstName() const { return firstName_; }
  QString lastName() const { return lastName_; }
  QString fullName() const { return firstName_ + " " + lastName_; }
  QString initials() const {
    QChar f = firstName_.isEmpty() ? QChar() : firstName_[0];
    QChar l = lastName_.isEmpty() ? QChar() : lastName_[0];
    return QString(f) + QString(l);
  }
  QString avatarColor() const { return generateAvatarColor(uuid_); }
  bool online() const { return status_ == "Online"; }
  QString status() const { return status_; }
  bool isMe() const { return isMe_; }
  QString token() const { return token_; }
  QString uuid() const { return uuid_; }
  QString avatar() const { return avatar_; }
  QString lastMessage() const { return lastMessage_; }
  QDateTime lastReadAt() const { return lastReadAt_; }
  int unreadCount() const { return unreadCount_; }

  // Setters
  void setEmail(const QString &v) { email_ = v; }
  void setFirstName(const QString &v) { firstName_ = v; }
  void setLastName(const QString &v) { lastName_ = v; }
  void setStatus(const QString &v) { status_ = v; }
  void setIsMe(bool v) { isMe_ = v; }
  void setToken(const QString &v) { token_ = v; }
  void setUuid(const QString &v) { uuid_ = v; }
  void setAvatar(const QString &v) { avatar_ = v; }
  void setLastMessage(const QString &v) { lastMessage_ = v; }
  void setUnreadCount(int v) { unreadCount_ = v; }
  void setLastReadAt(const QDateTime &v) { lastReadAt_ = v.toUTC(); }
  void clearToken() { token_.clear(); }
  bool isValid() const { return !email_.isEmpty() && !firstName_.isEmpty() && !uuid_.isEmpty(); }

  QJsonObject toJson() const;
  static User fromJson(const QJsonObject &json);

  bool operator==(const User &other) const { return uuid_ == other.uuid_; }

  void print() const {
    qDebug() << "{ email:" << email_ << ", name:" << fullName() << ", status:" << status_ << ", uuid:" << uuid_
             << ", isMe:" << isMe_ << "}";
  }

private:
  static QString generateAvatarColor(const QString &seed) {
    static const QStringList colors = {
        "#FF6B6B", "#FF9F43", "#FECA57", "#48DBFB", "#1DD1A1", "#0A84FF", "#BF5AF2", "#FF375F",
    };
    if (seed.isEmpty())
      return colors[0];
    uint hash = qHash(seed);
    return colors[hash % colors.size()];
  }

  QString email_;
  QString firstName_;
  QString lastName_;
  QString status_;
  bool isMe_ = false;
  QString token_;
  QString uuid_;
  QString avatar_;
  QString lastMessage_;
  QDateTime lastReadAt_;
  int unreadCount_ = 0;
};

#endif