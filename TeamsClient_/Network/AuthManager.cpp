#include "AuthManager.h"
#include <iostream>
#include <boost/asio/connect.hpp>
#include <boost/asio/write.hpp>

using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;

AuthManager::AuthManager(boost::asio::io_context& io,
                         const std::string& host,
                         unsigned short port,
                         QObject* parent)
    : QObject(parent),
      ctx_(ssl::context::tlsv12_client),
      socket_(io, ctx_),
      resolver_(io),
      host_(host),
      port_(port),
      connected_(false)
{
    try {
        ctx_.set_default_verify_paths();
        // ctx_.set_verify_mode(ssl::verify_peer);
        ctx_.set_verify_mode(boost::asio::ssl::verify_none);
    } catch (const std::exception& e) {
        qWarning() << "SSL context init error:" << e.what();
    }
}

AuthManager::~AuthManager() {
    disconnect();
}

void AuthManager::connectToServer() {
    try {
        auto endpoints = resolver_.resolve(host_, std::to_string(port_));
        boost::asio::connect(socket_.lowest_layer(), endpoints);
        socket_.handshake(ssl::stream_base::client);
        connected_ = true;
        qDebug() << "Secure connection established with auth server.";
    } catch (const std::exception& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

void AuthManager::authenticate(const QString& username, const QString& password, bool isLogin) {
    if (!connected_) {
        emit errorOccurred("Not connected to server.");
        return;
    }

    try {
        // On adapte le JSON pour utiliser "type" à la place de "action"
        std::string type = isLogin ? "login" : "register";
        std::string message = "{"
                              "\"type\":\"" + type + "\","
                              "\"username\":\"" + username.toStdString() + "\","
                              "\"password\":\"" + password.toStdString() + "\""
                              "}\n";

        boost::asio::write(socket_, boost::asio::buffer(message));

        readResponse();
    } catch (const std::exception& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

void AuthManager::readResponse() {
    try {
        char buffer[512];
        boost::system::error_code ec;
        std::size_t len = socket_.read_some(boost::asio::buffer(buffer), ec);

        if (ec) {
            emit errorOccurred(QString::fromStdString(ec.message()));
            return;
        }

        std::string response(buffer, len);
        if (response.find("success") != std::string::npos)
            emit authSuccess();
        else
            emit authFailed(QString::fromStdString(response));
    } catch (const std::exception& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

void AuthManager::disconnect() {
    if (connected_) {
        boost::system::error_code ec;
        socket_.shutdown();
        socket_.lowest_layer().close(ec);
        connected_ = false;
        qDebug() << "Disconnected from server.";
    }
}