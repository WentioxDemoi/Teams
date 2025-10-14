#include "DatabaseManager.h"
#include <iostream>

DatabaseManager::~DatabaseManager() {
    close();
}


bool DatabaseManager::open(const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (sqlite3_open(filename.c_str(), &m_db) != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    return true;
}

bool DatabaseManager::execute(const std::string& sql) {
    std::lock_guard<std::mutex> lock(m_mutex);
    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << (errMsg ? errMsg : "unknown") << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

std::vector<std::string> DatabaseManager::getAllMessages() {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> messages;

    const char* sql = "SELECT text FROM messages;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return messages;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* text = sqlite3_column_text(stmt, 0);
        if (text)
            messages.emplace_back(reinterpret_cast<const char*>(text));
    }

    sqlite3_finalize(stmt);
    return messages;
}

void DatabaseManager::close() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}