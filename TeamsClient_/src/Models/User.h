#ifndef USER_H
#define USER_H

#include "../includes.h"

/**
 * @class User
 * @brief Représente un utilisateur de l'application.
 *
 * Contient les informations essentielles comme l'ID, l'email, le nom d'utilisateur,
 * et le statut. Fournit des getters/setters, la sérialisation vers JSON
 * et une méthode pour vérifier la validité de l'objet.
 */
class User
{
public:
  User() = default;
  User(int id, const QString &email, const QString &username,
       const QString &status, bool isMe);

  // Getters
  int id() const { return id_; }
  QString email() const { return email_; }
  QString username() const { return username_; }
  QString status() const { return status_; }
  bool isMe() const { return isMe_; }

  // Setters
  void setId(int id) { id_ = id; }
  void setEmail(const QString &email) { email_ = email; }
  void setUsername(const QString &username) { username_ = username; }
  void setStatus(const QString &status) { status_ = status; }
  void isMe(bool isMe) { isMe_ = isMe; }

  // Serialization
  QJsonObject toJson() const;
  static User fromJson(const QJsonObject &json);

  bool isValid() const { return id_ > 0 && !email_.isEmpty(); }
  void print() const
  {
    qDebug() << "User {"
             << "id:" << id_
             << ", email:" << email_
             << ", username:" << username_
             << ", status:" << status_
             << ", status:" << isMe_
             << "}";
  }

private:
  int id_ = 0;
  QString email_;
  QString username_;
  QString status_;
  bool isMe_ = false;
};

// Q_DECLARE_METATYPE(User) // Attention, type User peut être à déclarer dans le futur si opérations plus complexes

#endif