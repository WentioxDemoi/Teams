#include "AuthRepository.h"

AuthRepository &AuthRepository::instance() {
  static AuthRepository instance;
  return instance;
}

AuthRepository::AuthRepository() : ctx_(DatabaseContext::instance()) {}

std::optional<User> AuthRepository::authenticate(const std::string &email,
                                                 const std::string &password,
                                                 bool is_register,
                                                 const std::optional<std::string> &username) {
    auto conn = ctx_.acquire();
    pqxx::work txn(*conn);

    try {
        const std::string token = RepositoriesTools::generate_token();
        const auto expiry = token_expiry_time();
        const std::string uuid = RepositoriesTools::generate_uuid();

        if (is_register) {
            auto exists = txn.exec_params(
                "SELECT id FROM " + ctx_.users_table() + " WHERE email = $1", email);

            if (!exists.empty()) {
                ctx_.release(conn);
                return std::nullopt;
            }

            auto hash = RepositoriesTools::hash_password(password);

            auto res = txn.exec_params(
                "INSERT INTO " + ctx_.users_table() +
                " (email, username, password_hash, token, token_expires_at, status, uuid) "
                "VALUES ($1,$2,$3,$4,$5,'online',$6) RETURNING id",
                email, username.value_or(""), hash, token,
                DatabaseTools::time_point_to_string(expiry),
                uuid
            );

            txn.commit();
            ctx_.release(conn);
            return User{res[0][0].as<int>(), username.value_or(""), email, token, "online", uuid};
        }

        auto res = txn.exec_params("SELECT id, password_hash, username, uuid FROM " +
                                   ctx_.users_table() + " WHERE email = $1", email);

        if (res.empty() || !RepositoriesTools::verify_password(password, res[0]["password_hash"].as<std::string>())) {
            ctx_.release(conn);
            return std::nullopt;
        }

        int user_id = res[0]["id"].as<int>();
        std::string db_username = res[0]["username"].as<std::string>();
        std::string db_uuid = res[0]["uuid"].as<std::string>();

        txn.exec_params(
            "UPDATE " + ctx_.users_table() +
            " SET token=$1, token_expires_at=$2, status='online', last_seen=NOW() "
            "WHERE id=$3",
            token, DatabaseTools::time_point_to_string(expiry), user_id
        );

        txn.commit();
        ctx_.release(conn);
        return User{user_id, db_username, email, token, "online", db_uuid};

    } catch (...) {
        ctx_.release(conn);
        throw;
    }
}

std::optional<User> AuthRepository::get_user_by_token(const std::string &token) {
    try {
        auto conn = ctx_.acquire();
        pqxx::work txn(*conn);

        pqxx::result res = txn.exec_params(
            "SELECT id, username, email, status, uuid "
            "FROM " + ctx_.users_table() +
            " WHERE token = $1 AND token_expires_at > NOW()",
            token
        );

        if (res.empty()) {
            ctx_.release(conn);
            return std::nullopt;
        }

        ctx_.release(conn);
        return User{
            res[0]["id"].as<int>(),
            res[0]["username"].as<std::string>(),
            res[0]["email"].as<std::string>(),
            token,
            res[0]["status"].as<std::string>(),
            res[0]["uuid"].as<std::string>()
        };
    } catch (const std::exception &e) {
        std::cerr << "Exception get_user_by_token: " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::chrono::system_clock::time_point AuthRepository::token_expiry_time() const {
    std::string expiry = std::getenv("EXPIRY_TIME_TOKEN");
    if (expiry.empty())
        throw std::runtime_error("Expiration du token non trouvée dans le .env");
    return std::chrono::system_clock::now() + std::chrono::hours(std::stoi(expiry));
}