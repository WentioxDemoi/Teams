#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/thread_pool.hpp>
#include <memory>
#include <vector>
#include <array>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
namespace ssl = asio::ssl;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket,
                 ssl::context& ssl_ctx,
                 asio::io_context& ioc,
                 asio::thread_pool& db_pool)
    : ssl_stream_(std::move(socket), ssl_ctx),
      io_context_(ioc),
      db_pool_(db_pool)
{}

    void start();

private:
    void do_read();
    void do_write();

    ssl::stream<tcp::socket> ssl_stream_;
    asio::io_context& io_context_;
    asio::thread_pool& db_pool_;

    std::array<char, 8192> buffer_;
    std::vector<std::string> write_queue_;
};

#endif