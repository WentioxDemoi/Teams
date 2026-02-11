#include "AuthService.h"

std::optional<User> AuthService::loginUser(const std::string &email,
                                           const std::string &password) {
  std::optional<User> user_ = userRepo_->find_by_email(email);
  if (!user_)
    return std::nullopt;

  if (Crypto::verify_password(password, user_->password_hash)) {

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
    return user_;
  }

  return std::nullopt;
}

std::optional<User> AuthService::registerUser(const std::string &username,
                                              const std::string &email,
                                              const std::string &password) {
  std::optional<User> user_ = userRepo_->find_by_email(email);
  if (!user_) {
    User user(Crypto::generate_uuid(), username, email,
              Crypto::hash_password(password), Crypto::generate_token(),
              config_.token_expiry_time(), "Online", config_.time(),
              config_.time());

    if (userRepo_->create(user)) {
      user.password_hash.clear();
      return user;
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