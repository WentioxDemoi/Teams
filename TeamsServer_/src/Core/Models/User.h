#ifndef USER_H
#define USER_H

#include "../../includes.h"
#include "../../Utils/PacketHelper.h"
#include <pqxx/pqxx>
#include "../../Utils/Config.h"

struct User {
  std::string uuid;
  std::string firstName;
  std::string lastName;
  std::string email;
  std::string password_hash;
  std::string plain_password;  // mot de passe en clair, transitoire — JAMAIS persisté ni renvoyé au client
  std::string token;
  std::chrono::system_clock::time_point token_expires_at;
  std::chrono::system_clock::time_point last_seen;
  std::chrono::system_clock::time_point created_at;

  User() = default;

  User(std::string uuid_, std::string firstName_, std::string lastName_,
       std::string email_, std::string password_hash_, std::string token_,
       std::chrono::system_clock::time_point token_expires_at_,
 std::chrono::system_clock::time_point last_seen_,
       std::chrono::system_clock::time_point created_at_)
      : uuid(std::move(uuid_)),
        firstName(std::move(firstName_)),
        lastName(std::move(lastName_)),
        email(std::move(email_)),
        password_hash(std::move(password_hash_)),
        token(std::move(token_)),
        token_expires_at(std::move(token_expires_at_)),
        last_seen(std::move(last_seen_)),
        created_at(std::move(created_at_)) {}
};

// Parsing d'un payload JSON venant du CLIENT (login / register).
// Ne remplit que ce que le client peut légitimement envoyer :
// email, firstName, lastName (vide pour login), et plain_password.
// uuid/token/dates sont attribués côté serveur, jamais lus ici.
inline User user_from_json(const std::string& json) {
    User user;
    user.email          = PacketHelper::extractValue(json, "email");
    user.firstName       = PacketHelper::extractValue(json, "firstName");
    user.lastName        = PacketHelper::extractValue(json, "lastName");
    user.plain_password   = PacketHelper::extractValue(json, "password");
    return user;
}

inline User user_from_db_row(const pqxx::row& row) {
    User user;
    user.uuid             = row["uuid"].as<std::string>();
    user.firstName        = row["first_name"].as<std::string>();
    user.lastName         = row["last_name"].as<std::string>();
    user.email            = row["email"].as<std::string>();
    user.password_hash    = row["password_hash"].as<std::string>();
    user.token            = row["token"].is_null() ? "" : row["token"].as<std::string>();
    user.created_at     = Config::instance().string_to_time_point(row["created_at"].as<std::string>());
    user.last_seen      = Config::instance().string_to_time_point(row["last_seen"].as<std::string>());
    user.token_expires_at = Config::instance().string_to_time_point(row["token_expires_at"].as<std::string>());
    return user;
}

#endif