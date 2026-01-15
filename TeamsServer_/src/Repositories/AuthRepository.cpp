#include "AuthRepository.h"

AuthRepository &AuthRepository::instance() {
  static AuthRepository instance;
  return instance;
}

AuthRepository::AuthRepository() : ctx_(DatabaseContext::instance()) {}

std::optional<User>
AuthRepository::authenticate(const std::string &email,
                             const std::string &password, bool is_register,
                             const std::optional<std::string> &username) {

  auto conn = ctx_.acquire();
  pqxx::work txn(*conn);

  try {
    const std::string token = RepositoriesTools::generate_token();
    const auto expiry = token_expiry_time();

    if (is_register) {
      auto exists = txn.exec_params(
          "SELECT id FROM " + ctx_.users_table() + " WHERE email = $1", email);

      if (!exists.empty()) {
        ctx_.release(conn);
        return std::nullopt;
      }

      auto hash = RepositoriesTools::hash_password(password);

      auto res =
          txn.exec_params("INSERT INTO " + ctx_.users_table() +
                              " (email, username, password_hash, token, "
                              "token_expires_at, status) "
                              "VALUES ($1,$2,$3,$4,$5,'online') RETURNING id",
                          email, username.value_or(""), hash, token,
                          DatabaseTools::time_point_to_string(expiry));

      txn.commit();
      ctx_.release(conn);
      std::string final_username = username.value_or("");
      return User{res[0][0].as<int>(), final_username, email, token, "online"};
    }

    auto res = txn.exec_params("SELECT id, password_hash, username FROM " +
                                   ctx_.users_table() + " WHERE email = $1",
                               email);

    if (res.empty()) {
      std::cout << "User non trouvé" << std::endl;
      ctx_.release(conn);
      return std::nullopt;
    }

    if (!RepositoriesTools::verify_password(
            password, res[0]["password_hash"].as<std::string>())) {
      std::cout << "Mdp non correspondant" << std::endl;
      ctx_.release(conn);
      return std::nullopt;
    }

    int user_id = res[0]["id"].as<int>();
    std::string db_username = res[0]["username"].as<std::string>();

    txn.exec_params("UPDATE " + ctx_.users_table() +
                        " SET token=$1, token_expires_at=$2, status='online', "
                        "last_seen=NOW() "
                        "WHERE id=$3",
                    token, DatabaseTools::time_point_to_string(expiry),
                    user_id);

    txn.commit();
    ctx_.release(conn);
    return User{user_id, db_username, email, token, "online"};
  } catch (...) {
    // Gestion du catch a changer
    ctx_.release(conn);
    throw;
  }
}

std::optional<User>
AuthRepository::get_user_by_token(const std::string &token) {
  auto conn = ctx_.acquire(); // pool_->get_connection();
  pqxx::work txn(*conn);

  pqxx::result res = txn.exec_params(
      "SELECT id, email, username, status "
      "FROM " +
          ctx_.users_table() + " WHERE token = $1 AND token_expires_at > NOW()",
      token);

  ctx_.release(conn);

  if (res.empty())
    return std::nullopt;

  return User{res[0]["id"].as<int>(), res[0]["email"].as<std::string>(),
              res[0]["username"].as<std::string>(), token,
              res[0]["status"].as<std::string>()};
}

std::chrono::system_clock::time_point
AuthRepository::token_expiry_time() const {
  std::string expiry = std::getenv("EXPIRY_TIME_TOKEN");
  if (expiry.empty())
    throw std::runtime_error("Expiration du token non trouvée dans le .env");

  return std::chrono::system_clock::now() +
         std::chrono::hours(std::stoi(expiry));
}