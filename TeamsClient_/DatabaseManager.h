#pragma once
#include <sqlite3.h>
#include <string>
#include <mutex>
#include <vector>

class DatabaseManager {
public:

    bool open(const std::string& filename);
    bool execute(const std::string& sql);
    void close();
    std::vector<std::string> getAllMessages();

    DatabaseManager() = default;
    ~DatabaseManager();
private:

    sqlite3* m_db = nullptr;
    std::mutex m_mutex;
};