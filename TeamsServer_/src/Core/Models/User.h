#ifndef USER_H
#define USER_H

#include "../../includes.h"

struct User {
  std::string uuid;
  std::string firstName;
  std::string lastName;
  std::string email;
  std::string password_hash;
  std::string token;
  std::chrono::system_clock::time_point token_expires_at;
  std::string status;
  std::chrono::system_clock::time_point last_seen;
  std::chrono::system_clock::time_point created_at;

  User() = default;

  User(std::string uuid_, std::string firstName_, std::string lastName_,
       std::string email_, std::string password_hash_, std::string token_,
       std::chrono::system_clock::time_point token_expires_at_,
       std::string status_, std::chrono::system_clock::time_point last_seen_,
       std::chrono::system_clock::time_point created_at_)
      : uuid(std::move(uuid_)),
        firstName(std::move(firstName_)),
        lastName(std::move(lastName_)),
        email(std::move(email_)),
        password_hash(std::move(password_hash_)),
        token(std::move(token_)),
        token_expires_at(std::move(token_expires_at_)),
        status(std::move(status_)),
        last_seen(std::move(last_seen_)),
        created_at(std::move(created_at_)) {}
};

#endif

