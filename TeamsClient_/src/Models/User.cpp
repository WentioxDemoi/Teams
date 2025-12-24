#include "User.h"

User::User(int id,
           const QString& username,
           const QString& token,
           const QString& status)
    : id_(id),
      username_(username),
      token_(token),
      status_(status)
{
}

QJsonObject User::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["username"] = username_;
    json["token"] = token_;
    json["status"] = status_;
    return json;
}

User User::fromJson(const QJsonObject& json)
{
    User user;
    user.setId(json["id"].toInt());
    user.setUsername(json["username"].toString());
    user.setToken(json["token"].toString());
    user.setStatus(json["status"].toString());
    return user;
}