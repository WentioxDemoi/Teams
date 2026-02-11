#include "User.h"
#include <QJsonObject>

User::User(const QString &email, const QString &username,
           const QString &status, bool isMe, const QString &token, const QString &uuid)
    : email_(email), username_(username), status_(status),
      isMe_(isMe), token_(token), uuid_(uuid) {}

QJsonObject User::toJson() const {
    QJsonObject json;
    json["email"] = email_;
    json["username"] = username_;
    json["status"] = status_;
    json["isMe"] = isMe_;
    json["token"] = token_;
    json["uuid"] = uuid_;
    return json;
}

User User::fromJson(const QJsonObject &json) {
    User user;
    if (json.contains("email") && json["email"].isString()) user.setEmail(json["email"].toString());
    if (json.contains("username") && json["username"].isString()) user.setUsername(json["username"].toString());
    if (json.contains("status") && json["status"].isString()) user.setStatus(json["status"].toString());
    if (json.contains("isMe") && json["isMe"].isBool()) user.setIsMe(json["isMe"].toBool());
    if (json.contains("token") && json["token"].isString()) user.setToken(json["token"].toString());
    if (json.contains("uuid") && json["uuid"].isString()) user.setUuid(json["uuid"].toString());
    return user;
}