#include "ConnectionPool.h"

ConnectionPool::ConnectionPool(const std::string& conn_str, int size) {
    try {
    for(int i = 0; i < size; ++i) {
        pool_.emplace(std::make_shared<pqxx::connection>(conn_str));
    }
    std::cout << "Mettre message de confirmation de création de pool de connexion avec la db" << std::endl;
} catch(const std::exception &e) {
    std::cout << "Erreur lors de la création de la pool de connexion à la DB " << e.what() << std::endl;
}
}

std::shared_ptr<pqxx::connection> ConnectionPool::get_connection() {
    std::unique_lock<std::mutex> lock(mutex_);
    while(pool_.empty()) cv_.wait(lock);
    auto conn = pool_.front();
    pool_.pop();
    return conn;
}

void ConnectionPool::return_connection(std::shared_ptr<pqxx::connection> conn) {
    std::lock_guard<std::mutex> lock(mutex_);
    pool_.push(conn);
    cv_.notify_one();
}