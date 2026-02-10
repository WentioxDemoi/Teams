// infrastructure/database/DatabaseManager.cpp (simplifié)
#include "DatabaseManager.h"

DatabaseManager &DatabaseManager::instance() {
  static DatabaseManager instance;
  return instance;
}

DatabaseManager::DatabaseManager() : config_(Config::instance()) {
  initialize();
  initialize_schema();
}

void DatabaseManager::initialize() {
  if (initialized_) {
    std::cout << "[DatabaseManager] Already initialized" << std::endl;
    return;
  }

  try {
    const std::string connection_string =
        config_.db_url() + "/" + config_.db_name();

    // Create connection pool
    pool_ = std::make_unique<ConnectionPool>(connection_string,
                                             config_.db_pool_size());

    std::cout << "[DatabaseManager] Initialized successfully" << std::endl;
    std::cout << "  - Database: " << config_.db_name() << std::endl;
    std::cout << "  - Pool size: " << config_.db_pool_size() << std::endl;

    initialized_ = true;

  } catch (const std::exception &e) {
    std::cerr << "[DatabaseManager] Initialization failed: " << e.what()
              << std::endl;
    throw;
  }
}

void DatabaseManager::initialize_schema() {
  if (!initialized_) {
    throw std::runtime_error(
        "DatabaseManager must be initialized before creating schema");
  }

  try {
    auto &config = Config::instance();
    auto conn = acquire_connection();
    pqxx::work txn(*conn);

    std::cout << "[DatabaseManager] Creating database schema..." << std::endl;

    // Users table
    txn.exec("CREATE TABLE IF NOT EXISTS " + config.table_users() +
             " ("
             "email TEXT UNIQUE NOT NULL, "
             "username TEXT UNIQUE, "
             "password_hash TEXT NOT NULL, "
             "uuid TEXT UNIQUE, "
             "token TEXT, "
             "token_expires_at TIMESTAMP WITH TIME ZONE, "
             "status TEXT DEFAULT 'offline', "
             "last_seen TIMESTAMP WITH TIME ZONE DEFAULT NOW(), "
             "created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()"
             ");");

    // // Channels table
    // txn.exec("CREATE TABLE IF NOT EXISTS " + config.table_channels() +
    //          " ("
    //          "id SERIAL PRIMARY KEY, "
    //          "name TEXT NOT NULL, "
    //          "is_private BOOLEAN DEFAULT TRUE, "
    //          "created_at TIMESTAMP DEFAULT NOW()"
    //          ");");

    // // Channel members table
    // txn.exec("CREATE TABLE IF NOT EXISTS " + config.table_channel_members() +
    //          " ("
    //          "channel_id INT REFERENCES " + config.table_channels() +
    //          "(id) ON DELETE CASCADE, "
    //          "user_id INT REFERENCES " + config.table_users() +
    //          "(id) ON DELETE CASCADE, "
    //          "joined_at TIMESTAMP DEFAULT NOW(), "
    //          "PRIMARY KEY(channel_id, user_id)"
    //          ");");

    // Messages table
    // txn.exec("CREATE TABLE IF NOT EXISTS " + config.table_messages() +
    //          " ("
    //          "id SERIAL PRIMARY KEY, "
    //          "channel_id INT REFERENCES " + config.table_channels() +
    //          "(id) ON DELETE CASCADE, "
    //          "sender_id INT REFERENCES " + config.table_users() +
    //          "(id) ON DELETE SET NULL, "
    //          "content TEXT NOT NULL, "
    //          "message_type TEXT DEFAULT 'text', "
    //          "edited BOOLEAN DEFAULT FALSE, "
    //          "created_at TIMESTAMP DEFAULT NOW(), "
    //          "updated_at TIMESTAMP DEFAULT NOW()"
    //          ");");

    // Create indexes
    txn.exec("CREATE INDEX IF NOT EXISTS idx_users_email ON " +
             config.table_users() + "(email);");
    txn.exec("CREATE INDEX IF NOT EXISTS idx_users_token ON " +
             config.table_users() + "(token);");
    // txn.exec("CREATE INDEX IF NOT EXISTS idx_messages_channel ON " +
    //          config.table_messages() + "(channel_id);");
    // txn.exec("CREATE INDEX IF NOT EXISTS idx_messages_sender ON " +
    //          config.table_messages() + "(sender_id);");
    // txn.exec("CREATE INDEX IF NOT EXISTS idx_channel_members_user ON " +
    //          config.table_channel_members() + "(user_id);");

    txn.commit();
    release_connection(conn);

    std::cout << "[DatabaseManager] Schema created successfully" << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "[DatabaseManager] Schema creation failed: " << e.what()
              << std::endl;
    throw;
  }
}

std::shared_ptr<pqxx::connection> DatabaseManager::acquire_connection() {
  if (!initialized_) {
    throw std::runtime_error("DatabaseManager not initialized");
  }
  return pool_->get_connection();
}

void DatabaseManager::release_connection(
    std::shared_ptr<pqxx::connection> conn) {
  if (!initialized_) {
    throw std::runtime_error("DatabaseManager not initialized");
  }
  pool_->return_connection(conn);
}