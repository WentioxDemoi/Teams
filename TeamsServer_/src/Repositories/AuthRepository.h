#ifndef AUTHREPOSITORY_H
#define AUTHREPOSITORY_H

#include "../Structs/User.h"
#include "../Database/DatabaseContext.h"
#include "../Database/DatabaseTools.h"
#include "RepositoriesTools.h"
#include "../includes.h"

class AuthRepository {
public:
  static AuthRepository &instance();

  std::optional<User>
  authenticate(const std::string &email, const std::string &password,
               bool is_register,
               const std::optional<std::string> &username = std::nullopt);

  std::optional<User> get_user_by_token(const std::string &token);

private:
  DatabaseContext &ctx_;

  AuthRepository();
  ~AuthRepository() = default;

  std::chrono::system_clock::time_point token_expiry_time() const;

  AuthRepository(const AuthRepository &) = delete;
  AuthRepository(AuthRepository &&) = delete;
  AuthRepository &operator=(const AuthRepository &) = delete;
  AuthRepository &operator=(AuthRepository &&) = delete;
};

#endif