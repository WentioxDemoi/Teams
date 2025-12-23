#include "AuthRepository.h"
#include "DatabaseTools.h"
#include <argon2.h>

AuthRepository &AuthRepository::instance() {
  static AuthRepository instance;
  return instance;
}

AuthRepository::AuthRepository() : ctx_(DatabaseContext::instance()) {}

std::optional<User> AuthRepository::authenticate(const std::string &username,
                                                 const std::string &password,
                                                 bool is_register) {

  auto conn = ctx_.acquire();
  pqxx::work txn(*conn);

  try {
    const std::string token = generate_token();
    const auto expiry = token_expiry_time();

    if (is_register) {
      auto exists = txn.exec_params("SELECT id FROM " + ctx_.users_table() +
                                        " WHERE username = $1",
                                    username);

      if (!exists.empty()) {
        ctx_.release(conn);
        return std::nullopt;
      }

      auto hash = hash_password(password);

      auto res = txn.exec_params(
          "INSERT INTO " + ctx_.users_table() +
              " (username, password_hash, token, token_expires_at, status) "
              "VALUES ($1,$2,$3,$4,'online') RETURNING id",
          username, hash, token, DatabaseTools::time_point_to_string(expiry));

      txn.commit();
      ctx_.release(conn);
      return User{res[0][0].as<int>(), username, token, "online"};
    }

    auto res = txn.exec_params("SELECT id, password_hash FROM " +
                                   ctx_.users_table() + " WHERE username = $1",
                               username);

    if (res.empty()) {
      ctx_.release(conn);
      return std::nullopt;
    }

    if (!verify_password(password, res[0]["password_hash"].as<std::string>())) {
      ctx_.release(conn);
      return std::nullopt;
    }

    int user_id = res[0]["id"].as<int>();

    txn.exec_params("UPDATE " + ctx_.users_table() +
                        " SET token=$1, token_expires_at=$2, status='online', "
                        "last_seen=NOW() "
                        "WHERE id=$3",
                    token, DatabaseTools::time_point_to_string(expiry),
                    user_id);

    txn.commit();
    ctx_.release(conn);

    return User{user_id, username, token, "online"};

  } catch (...) {
    ctx_.release(conn);
    throw;
  }
}

std::string AuthRepository::hash_password(const std::string &password) {
  const uint32_t t_cost = 3;
  const uint32_t m_cost =
      1 << 16; // 64 MB | A augmenter/réduire en fonction des ressources/besoins
  const uint32_t parallelism = 1;
  const size_t salt_len = 16;
  const size_t hash_len = 32;

  std::vector<uint8_t> salt(salt_len);
  std::random_device rd;
  for (auto &b : salt)
    b = static_cast<uint8_t>(rd());

  // Calcul taille exacte de la string encodée
  size_t encoded_len = argon2_encodedlen(t_cost, m_cost, parallelism, salt_len,
                                         hash_len, Argon2_id);

  std::vector<char> encoded(encoded_len);

  int rc = argon2id_hash_encoded(t_cost, m_cost, parallelism, password.data(),
                                 password.size(), salt.data(), salt.size(),
                                 hash_len, encoded.data(), encoded.size());

  if (rc != ARGON2_OK)
    throw std::runtime_error(argon2_error_message(rc));

  return std::string(encoded.data());
}

bool AuthRepository::verify_password(const std::string &password,
                                     const std::string &hash) {
  return argon2id_verify(hash.c_str(), password.data(), password.size()) ==
         ARGON2_OK;
}

std::string AuthRepository::generate_token() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> dis(0, 15);

  std::stringstream ss;
  for (int i = 0; i < 32; ++i)
    ss << std::hex << dis(gen);
  return ss.str();
}

std::optional<User>
AuthRepository::get_user_by_token(const std::string &token) {
  auto conn = ctx_.acquire(); // pool_->get_connection();
  pqxx::work txn(*conn);

  pqxx::result res = txn.exec_params(
      "SELECT id, username, status "
      "FROM " +
          ctx_.users_table() + " WHERE token = $1 AND token_expires_at > NOW()",
      token);

  ctx_.release(conn);

  if (res.empty())
    return std::nullopt;

  return User{res[0]["id"].as<int>(), res[0]["username"].as<std::string>(),
              token, res[0]["status"].as<std::string>()};
}

std::chrono::system_clock::time_point
AuthRepository::token_expiry_time() const {
  std::string expiry = std::getenv("EXPIRY_TIME_TOKEN");
  if (expiry.empty())
    throw std::runtime_error("Expiration du token non trouvée dans le .env");

  return std::chrono::system_clock::now() +
         std::chrono::hours(std::stoi(expiry));
}