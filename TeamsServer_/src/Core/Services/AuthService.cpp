#include "AuthService.h"

std::optional<User> AuthService::loginUser(const User &user) {
  std::optional<User> user_ = userRepo_->find_by_email(user.email);
  if (!user_)
    return std::nullopt;

  if (Crypto::verify_password(user.plain_password, user_->password_hash)) {

    // Mettre à jour la date d'expiration du token
    user_->token_expires_at = config_.token_expiry_time();
    user_->token = Crypto::generate_token();
    bool updated = userRepo_->update(*user_);
    if (!updated) {
      std::cerr << "[AuthService] Warning: Failed to update token expiration "
                   "for user "
                << user_->uuid << std::endl;
    } else {
      std::cout << "[AuthService] Token expiration updated to "
                << config_.time_point_to_string(user_->token_expires_at)
                << " for user " << user_->uuid << std::endl;
    }
    user_->password_hash.clear();
    user_->plain_password.clear();
    return user_;
  }

  return std::nullopt;
}

std::optional<User> AuthService::registerUser(const User &user) {
  std::optional<User> existing = userRepo_->find_by_email(user.email);
  if (!existing) {
    User newUser(Crypto::generate_uuid_v4(), user.firstName, user.lastName,
                 user.email, Crypto::hash_password(user.plain_password),
                 Crypto::generate_token(), config_.token_expiry_time(),
                 config_.time(), config_.time());

    if (userRepo_->create(newUser)) {
      newUser.password_hash.clear();
      newUser.plain_password.clear();
      return newUser;
    }
  }
  return std::nullopt;
}

std::optional<User> AuthService::validateToken(const std::string &token) {
  std::optional<User> user_ = userRepo_->find_by_token(token);
  if (!user_)
    return std::nullopt;
  return user_;
}