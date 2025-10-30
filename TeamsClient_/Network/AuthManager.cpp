#include "AuthManager.h"
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

AuthManager::AuthManager(boost::asio::io_context& io, const std::string& host, unsigned short port, QObject* parent)
    : QObject(parent), socket_(io), resolver_(io), host_(host), port_(port), connected_(false)
{
}

AuthManager::~AuthManager() {
    disconnect();
}

void AuthManager::connectToServer() {
    try {
        auto endpoints = resolver_.resolve(host_, std::to_string(port_));
        boost::asio::connect(socket_, endpoints);
        connected_ = true;
        qDebug() << "Connected to auth server.";
        
    } catch (const std::exception& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

void AuthManager::authenticate(const QString& username, const QString& password, bool isLogin) {
    if (!connected_) {
        emit errorOccurred("Not connected to server.");
        return;
    }

    qDebug() << "ICIIII";

    try {
        std::string message;
        if (isLogin)
            message = "{\"action\":\"login\",\"username\":\"" +
                                username.toStdString() + "\",\"password\":\"" +
                                password.toStdString() + "\"}\n";
        else
            message = "{\"action\":\"login\",\"username\":\"" +
                                username.toStdString() + "\",\"password\":\"" +
                                password.toStdString() + "\"}\n";

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
            emit loginSuccess();
        else
            emit loginFailed(QString::fromStdString(response));

    } catch (const std::exception& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

void AuthManager::disconnect() {
    if (connected_) {
        boost::system::error_code ec;
        socket_.shutdown(tcp::socket::shutdown_both, ec);
        socket_.close(ec);
        connected_ = false;
    }
}