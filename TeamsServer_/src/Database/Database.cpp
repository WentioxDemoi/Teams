#include "Database.h"


void Database::init_from_env() {
    const char* env_db_url  = std::getenv("DB_URL");
    const char* env_db_name = std::getenv("DB_NAME");
    if(!env_db_url || !env_db_name) throw std::runtime_error("DB_URL ou DB_NAME non défini");

    db_url_  = env_db_url;
    db_name_ = env_db_name;

    table_users_          = std::getenv("TABLE_USERS")          ? std::getenv("TABLE_USERS")          : "users";
    table_channels_       = std::getenv("TABLE_CHANNELS")       ? std::getenv("TABLE_CHANNELS")       : "channels";
    table_channel_members_= std::getenv("TABLE_CHANNEL_MEMBERS")? std::getenv("TABLE_CHANNEL_MEMBERS"): "channel_members";
    table_messages_       = std::getenv("TABLE_MESSAGES")       ? std::getenv("TABLE_MESSAGES")       : "messages";

    // Nombre de connexions dans la pool
    const char* nb_conn_env = std::getenv("NB_CONN");
    int nb_conn = nb_conn_env ? std::stoi(nb_conn_env) : 4;

    pqxx::connection admin_conn(db_url_ + "/postgres"); 
    pqxx::work txn(admin_conn);

    try {
        txn.exec("CREATE DATABASE " + txn.quote_name(db_name_));
        std::cout << "Database '" << db_name_ << "' créée.\n";
        txn.commit();
    } catch(const pqxx::sql_error&) {
        std::cout << "Database '" << db_name_ << "' existe déjà.\n";
    }
    const std::string url = db_url_ + "/" + db_name_;
    pool_ = std::make_unique<ConnectionPool>(url , nb_conn);

    
    init_database();
}

void Database::init_database() {
    try {
        auto db_conn = pool_->get_connection();
        pqxx::work db_txn(*db_conn);

        db_txn.exec(
    "CREATE TABLE IF NOT EXISTS " + table_users_ +
    " (id SERIAL PRIMARY KEY, username TEXT UNIQUE NOT NULL, password_hash TEXT NOT NULL, "
    "token TEXT, status TEXT DEFAULT 'offline', last_seen TIMESTAMP DEFAULT NOW(), created_at TIMESTAMP DEFAULT NOW());");

        db_txn.exec("CREATE TABLE IF NOT EXISTS " + table_channels_ +
                    " (id SERIAL PRIMARY KEY, name TEXT NOT NULL, is_private BOOLEAN DEFAULT TRUE, created_at TIMESTAMP DEFAULT NOW());");

        db_txn.exec("CREATE TABLE IF NOT EXISTS " + table_channel_members_ +
                    " (channel_id INT REFERENCES " + table_channels_ + "(id), user_id INT REFERENCES " + table_users_ + "(id), PRIMARY KEY(channel_id, user_id));");

        db_txn.exec("CREATE TABLE IF NOT EXISTS " + table_messages_ +
                    " (id SERIAL PRIMARY KEY, channel_id INT REFERENCES " + table_channels_ + "(id), sender_id INT REFERENCES " + table_users_ + "(id), content TEXT NOT NULL, created_at TIMESTAMP DEFAULT NOW());");

        db_txn.commit();
        pool_->return_connection(db_conn);

        std::cout << "Base '" << db_name_ << "' et toutes les tables sont prêtes.\n";

    } catch(const std::exception& e) {
        std::cerr << "Erreur init_database: " << e.what() << std::endl;
    }
}

bool Database::add_user(const std::string& username, const std::string& password) {
    try {
        auto conn = pool_->get_connection();
        pqxx::work txn(*conn);

        // Hash du mot de passe
        std::string hash;
        unsigned char hash_bytes[SHA256_DIGEST_LENGTH];
        SHA256((unsigned char*)password.c_str(), password.size(), hash_bytes);
        std::stringstream ss;
        for(int i=0; i<SHA256_DIGEST_LENGTH; ++i) ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash_bytes[i];
        hash = ss.str();

        txn.exec_params("INSERT INTO " + table_users_ + " (username, password_hash) VALUES ($1, $2)", username, hash);
        txn.commit();
        pool_->return_connection(conn);

        return true;
    } catch(const std::exception& e) {
        std::cerr << "Erreur add_user: " << e.what() << std::endl;
        return false;
    }
}

std::optional<User> Database::authenticate_user(const std::string& username, const std::string& password) {
    try {
        auto conn = pool_->get_connection();
        pqxx::work txn(*conn);

        pqxx::result r = txn.exec_params("SELECT id, password_hash FROM " + table_users_ + " WHERE username=$1", username);
        if(r.size() != 1) return std::nullopt;

        std::string stored_hash = r[0]["password_hash"].c_str();
        std::string input_hash = hash_password(password);
        if(input_hash != stored_hash) return std::nullopt;

        std::string token = generate_token();
        txn.exec_params("UPDATE " + table_users_ + " SET token=$1 WHERE id=$2", token, r[0]["id"].as<int>());
        txn.commit();
        pool_->return_connection(conn);

        return User{ r[0]["id"].as<int>(), username, token };
    } catch(const std::exception& e) {
        std::cerr << "Erreur authenticate_user: " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<User> Database::get_user_by_token(const std::string& token) {
    try {
        auto conn = pool_->get_connection();
        pqxx::work txn(*conn);

        pqxx::result r = txn.exec_params("SELECT id, username FROM " + table_users_ + " WHERE token=$1", token);
        if(r.size() != 1) return std::nullopt;

        pool_->return_connection(conn);
        return User{ r[0]["id"].as<int>(), r[0]["username"].c_str(), token };
    } catch(const std::exception& e) {
        std::cerr << "Erreur get_user_by_token: " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::string Database::hash_password(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)password.c_str(), password.size(), hash);

    std::stringstream ss;
    for(int i=0; i<SHA256_DIGEST_LENGTH; ++i) ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return ss.str();
}

std::string Database::generate_token() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);

    std::stringstream ss;
    for(int i=0; i<32; ++i) ss << std::hex << dis(gen);
    return ss.str();
}