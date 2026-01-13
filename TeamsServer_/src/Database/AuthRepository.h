#ifndef AUTHREPOSITORY_H
#define AUTHREPOSITORY_H

#include "../Structs/User.h"
#include "DatabaseContext.h"

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

  // Constructeur privé
  AuthRepository();
  ~AuthRepository() = default;

  std::string hash_password(const std::string &password);
  bool verify_password(const std::string &password, const std::string &hash);
  std::string generate_token();
  std::chrono::system_clock::time_point token_expiry_time() const;

  AuthRepository(const AuthRepository &) = delete;
  AuthRepository(AuthRepository &&) = delete;
  AuthRepository &operator=(const AuthRepository &) = delete;
  AuthRepository &operator=(AuthRepository &&) = delete;
};

#endif