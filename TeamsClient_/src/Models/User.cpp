#include "User.h"

User::User(int id, const QString &email, const QString &username,
           const QString &status, bool isMe)
    : id_(id), email_(email), username_(username),
      status_(status), isMe_(isMe) {}

QJsonObject User::toJson() const
{
  QJsonObject json;
  json["id"] = id_;
  json["email"] = email_;
  json["username"] = username_;
  json["status"] = status_;
  json["isMe"] = isMe_;
  return json;
}

User User::fromJson(const QJsonObject &json)
{
  User user;

  if (json.contains("id") && json["id"].isDouble())
    user.setId(json["id"].toInt());

  if (json.contains("email") && json["email"].isString())
    user.setEmail(json["email"].toString());

  if (json.contains("username") && json["username"].isString())
    user.setUsername(json["username"].toString());

  if (json.contains("status") && json["status"].isString())
    user.setStatus(json["status"].toString());

  return user;
}