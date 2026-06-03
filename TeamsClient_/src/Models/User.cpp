#include "User.h"

User::User(const QString& email, const QString& firstName, const QString& lastName,
           const QString& status, bool isMe, const QString& token,
           const QString& uuid, const QString& avatar, const QString& lastMessage)
    : email_(email),
      firstName_(firstName),
      lastName_(lastName),
      status_(status),
      isMe_(isMe),
      token_(token),
      uuid_(uuid),
      avatar_(avatar),
      lastMessage_(lastMessage) {}

QJsonObject User::toJson() const {
  QJsonObject json;
  json["email"]       = email_;
  json["firstName"]   = firstName_;
  json["lastName"]    = lastName_;
  json["status"]      = status_;
  json["isMe"]        = isMe_;
  json["token"]       = token_;
  json["uuid"]        = uuid_;
  json["avatar"]      = avatar_;
  return json;
}

User User::fromJson(const QJsonObject& json) {
  User user;
  if (json.contains("email")     && json["email"].isString())     user.setEmail(json["email"].toString());
  if (json.contains("firstName") && json["firstName"].isString()) user.setFirstName(json["firstName"].toString());
  if (json.contains("lastName")  && json["lastName"].isString())  user.setLastName(json["lastName"].toString());
  if (json.contains("status")    && json["status"].isString())    user.setStatus(json["status"].toString());
  if (json.contains("isMe")      && json["isMe"].isBool())        user.setIsMe(json["isMe"].toBool());
  if (json.contains("token")     && json["token"].isString())     user.setToken(json["token"].toString());
  if (json.contains("uuid")      && json["uuid"].isString())      user.setUuid(json["uuid"].toString());
  if (json.contains("avatar")    && json["avatar"].isString())    user.setAvatar(json["avatar"].toString());
  return user;
}