#ifndef CONNECTIONPOOL_H
#define CONNECTIONPOOL_H

#include "../includes.h"

class ConnectionPool {
public:
    ConnectionPool(const std::string& conn_str, int size);

    std::shared_ptr<pqxx::connection> get_connection();
    void return_connection(std::shared_ptr<pqxx::connection> conn);

private:
    std::queue<std::shared_ptr<pqxx::connection>> pool_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

#endif