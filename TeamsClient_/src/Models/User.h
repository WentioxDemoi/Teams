#ifndef USER_H
#define USER_H

#include "../includes.h"

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
  User(int id, const QString &email, const QString &username,
       const QString &status, bool isMe, const QString &token = QString());

  // Getters
  int id() const { return id_; }
  QString email() const { return email_; }
  QString username() const { return username_; }
  QString status() const { return status_; }
  bool isMe() const { return isMe_; }
  QString token() const { return token_; }
  void clearToken() { token_.clear(); }

  // Setters
  void setId(int id) { id_ = id; }
  void setEmail(const QString &email) { email_ = email; }
  void setUsername(const QString &username) { username_ = username; }
  void setStatus(const QString &status) { status_ = status; }
  void setIsMe(bool isMe) { isMe_ = isMe; }
  void setToken(const QString &token) { token_ = token; }

  // Serialization
  QJsonObject toJson() const;
  static User fromJson(const QJsonObject &json);

  bool isValid() const { return id_ > 0 && !email_.isEmpty(); }

  void print() const {
    qDebug() << "User {"
             << "id:" << id_ << ", email:" << email_
             << ", username:" << username_ << ", status:" << status_
             << ", isMe:" << isMe_ << "}";
  }

private:
  int id_ = 0;
  QString email_;
  QString username_;
  QString status_;
  bool isMe_ = false;
  QString token_;
};

#endif