#include "Session.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp> 

// Session::Session(tcp::socket socket,
//                  ssl::context& ssl_ctx,
//                  asio::io_context& ioc,
//                  asio::thread_pool& db_pool)
//     : ssl_stream_(std::move(socket), ssl_ctx),
//       io_context_(ioc),
//       db_pool_(db_pool)
// {}

void Session::start() {
    auto self = shared_from_this();
    ssl_stream_.async_handshake(ssl::stream_base::server,
        [this, self](const boost::system::error_code& ec) {
            if (ec) {
                std::cerr << "Handshake failed: " << ec.message() << "\n";
                return;
            }
            do_read();
        });
}

void Session::do_read() {
    auto self = shared_from_this();
    ssl_stream_.async_read_some(asio::buffer(buffer_),
        [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec) {
                if (ec != asio::error::eof && ec != asio::ssl::error::stream_truncated)
                    std::cerr << "Read error: " << ec.message() << "\n";
                return;
            }

            std::string request(buffer_.data(), bytes_transferred);

            asio::post(db_pool_, [this, self, req = std::move(request)]() mutable {
                nlohmann::json response_json;
                try {
                    auto j = nlohmann::json::parse(req);

                    if (j.contains("action") && j.contains("username") && j.contains("password")) {
                        std::string action = j["action"];
                        std::string username = j["username"];
                        std::string password = j["password"];

                        if (action == "register") {
                            bool success = db_.add_user(username, password);
                            response_json["status"] = success ? "ok" : "error";
                            response_json["message"] = success ? "User registered" : "Username already exists";
                        } 
                        else if (action == "login") {
                            auto user = db_.authenticate_user(username, password);
                            if (user.has_value()) {
                                response_json["status"] = "ok";
                                response_json["token"] = user->token;
                            } else {
                                response_json["status"] = "error";
                                response_json["message"] = "Invalid credentials";
                            }
                        } else {
                            response_json["status"] = "error";
                            response_json["message"] = "Unknown action";
                        }
                    } else {
                        response_json["status"] = "error";
                        response_json["message"] = "Invalid request format";
                    }
                } catch (const std::exception& e) {
                    response_json["status"] = "error";
                    response_json["message"] = e.what();
                }

                std::string db_result = response_json.dump() + "\n";

                asio::post(io_context_, [this, self, res = std::move(db_result)]() {
                    write_queue_.push_back(res);
                    if (write_queue_.size() == 1)
                        do_write();
                });
            });

            do_read();
        });
}

void Session::do_write() {
    if (write_queue_.empty()) return;

    auto self = shared_from_this();
    const std::string& msg = write_queue_.front();

    asio::async_write(ssl_stream_, asio::buffer(msg),
        [this, self](const boost::system::error_code& ec, std::size_t) {
            if (ec) {
                std::cerr << "Write error: " << ec.message() << "\n";
                return;
            }

            write_queue_.erase(write_queue_.begin());
            if (!write_queue_.empty())
                do_write();
        });
}