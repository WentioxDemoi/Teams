#include "Database.h"

void Database::init_database() {
    try {
        // 1️⃣ Connexion à "postgres" pour créer la DB si elle n'existe pas
        pqxx::connection conn(db_url_);
        pqxx::work txn(conn);

        try {
            txn.exec("CREATE DATABASE " + txn.quote_name(db_name_));
            std::cout << "Database '" << db_name_ << "' créée.\n";
        } catch (const pqxx::sql_error& e) {
            std::cout << "Database '" << db_name_ << "' existe déjà.\n";
        }
        txn.commit();

        // 2️⃣ Connexion à la DB cible
        pqxx::connection db_conn(db_url_ + " dbname=" + db_name_);
        pqxx::work db_txn(db_conn);

        // 3️⃣ Création de toutes les tables si elles n'existent pas

        // Table users
        db_txn.exec(
            "CREATE TABLE IF NOT EXISTS users ("
            "id SERIAL PRIMARY KEY, "
            "username TEXT UNIQUE NOT NULL, "
            "password_hash TEXT NOT NULL, "
            "token TEXT, "
            "created_at TIMESTAMP DEFAULT NOW()"
            ");"
        );

        // Table channels
        db_txn.exec(
            "CREATE TABLE IF NOT EXISTS channels ("
            "id SERIAL PRIMARY KEY, "
            "name TEXT NOT NULL, "
            "is_private BOOLEAN DEFAULT TRUE, "
            "created_at TIMESTAMP DEFAULT NOW()"
            ");"
        );

        // Table channel_members
        db_txn.exec(
            "CREATE TABLE IF NOT EXISTS channel_members ("
            "channel_id INT REFERENCES channels(id), "
            "user_id INT REFERENCES users(id), "
            "PRIMARY KEY(channel_id, user_id)"
            ");"
        );

        // Table messages
        db_txn.exec(
            "CREATE TABLE IF NOT EXISTS messages ("
            "id SERIAL PRIMARY KEY, "
            "channel_id INT REFERENCES channels(id), "
            "sender_id INT REFERENCES users(id), "
            "content TEXT NOT NULL, "
            "created_at TIMESTAMP DEFAULT NOW()"
            ");"
        );

        db_txn.commit();
        std::cout << "Base '" << db_name_ << "' et toutes les tables sont prêtes.\n";

    } catch (const std::exception& e) {
        std::cerr << "Erreur init_database: " << e.what() << std::endl;
    }
}

bool Database::add_user(const std::string& username, const std::string& password) {
    try {
        pqxx::connection conn(db_url_ + " dbname=" + db_name_);
        pqxx::work txn(conn);

        std::string hash = hash_password(password);

        txn.exec_params("INSERT INTO users (username, password_hash) VALUES ($1, $2)", username, hash);
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Erreur add_user: " << e.what() << std::endl;
        return false;
    }
}

std::optional<User> Database::authenticate_user(const std::string& username, const std::string& password) {
    try {
        pqxx::connection conn(db_url_ + " dbname=" + db_name_);
        pqxx::work txn(conn);

        pqxx::result r = txn.exec_params("SELECT id, password_hash FROM users WHERE username=$1", username);
        if (r.size() != 1) return std::nullopt;

        std::string stored_hash = r[0]["password_hash"].c_str();
        if (hash_password(password) != stored_hash) return std::nullopt;

        std::string token = generate_token();
        txn.exec_params("UPDATE users SET token=$1 WHERE id=$2", token, r[0]["id"].as<int>());
        txn.commit();

        return User{ r[0]["id"].as<int>(), username, token };
    } catch (const std::exception& e) {
        std::cerr << "Erreur authenticate_user: " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<User> Database::get_user_by_token(const std::string& token) {
    try {
        pqxx::connection conn(db_url_ + " dbname=" + db_name_);
        pqxx::work txn(conn);

        pqxx::result r = txn.exec_params("SELECT id, username FROM users WHERE token=$1", token);
        if (r.size() != 1) return std::nullopt;

        return User{ r[0]["id"].as<int>(), r[0]["username"].c_str(), token };
    } catch (const std::exception& e) {
        std::cerr << "Erreur get_user_by_token: " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::string Database::hash_password(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)password.c_str(), password.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

    return ss.str();
}

std::string Database::generate_token() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);

    std::stringstream ss;
    for (int i = 0; i < 32; ++i) {
        ss << std::hex << dis(gen);
    }
    return ss.str();
}