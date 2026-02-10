// core/models/User.h
#ifndef USER_H
#define USER_H

#include "../../includes.h"

/**
 * @struct User
 * @brief Représente un utilisateur du système avec ses informations
 * essentielles.
 *
 * Contient les identifiants, les informations de connexion et l'état de
 * l'utilisateur, ainsi que des métadonnées sur les dates importantes (création,
 * dernière connexion, expiration du token). Constructeur par défaut et
 * constructeur paramétré disponibles.
 */
struct User {
  std::string uuid;
  std::string username;
  std::string email;
  std::string password_hash;
  std::string token;
  std::chrono::system_clock::time_point token_expires_at;
  std::string status;
  std::chrono::system_clock::time_point last_seen;
  std::chrono::system_clock::time_point created_at;

  User() = default;

  User(std::string uuid_, std::string username_, std::string email_,
       std::string password_hash_, std::string token_,
       std::chrono::system_clock::time_point token_expires_at_,
       std::string status_, std::chrono::system_clock::time_point last_seen_,
       std::chrono::system_clock::time_point created_at_)
      : uuid(std::move(uuid_)), username(std::move(username_)),
        email(std::move(email_)), password_hash(std::move(password_hash_)),
        token(std::move(token_)),
        token_expires_at(std::move(token_expires_at_)),
        status(std::move(status_)), last_seen(std::move(last_seen_)),
        created_at(std::move(created_at_)) {}
};

#endif