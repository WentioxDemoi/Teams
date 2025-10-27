#include "NetworkManager.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <QThread>

using boost::asio::ip::tcp;

NetworkManager::NetworkManager(const std::string& host, QObject *parent)
    : QObject(parent), socket_(io_context_), host_(host), running_(false)
{
}

NetworkManager::~NetworkManager()
{
}

void NetworkManager::start()
{
    if (running_) return;

    running_ = true;
    run();
}

void NetworkManager::run()
{
    try {
        tcp::resolver resolver(io_context_);
        tcp::resolver::results_type endpoints = resolver.resolve(host_, std::to_string(port_));
        boost::asio::connect(socket_, endpoints);

        emit connected();
        std::cout << "Connecté au serveur " << host_ << ":" << port_ << std::endl;

        while (running_) {
            char buffer[1024];
            boost::system::error_code ec;
            std::size_t len = socket_.read_some(boost::asio::buffer(buffer), ec);

            if (ec == boost::asio::error::eof) {
                break;
            } else if (ec) {
                emit errorOccurred(QString::fromStdString(ec.message()));
                break;
            }

            QString msg = QString::fromStdString(std::string(buffer, len));
            emit messageReceived(msg);
        }
    }
    catch (const std::exception& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
        std::cerr << "Erreur : " << e.what() << std::endl;
    }

    stop();
}

void NetworkManager::sendMessage(const QString& message)
{
    if (!socket_.is_open()) {
        emit errorOccurred("Socket fermé, impossible d'envoyer");
        return;
    }

    try {
        boost::asio::write(socket_, boost::asio::buffer(message.toStdString()));
        std::cout << "Message envoyé : " << message.toStdString() << std::endl;
    }
    catch (const std::exception& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
        std::cerr << "Erreur lors de l'envoi : " << e.what() << std::endl;
    }
}

void NetworkManager::stop()
{
    running_ = false;

    if (socket_.is_open()) {
        boost::system::error_code ec;
        socket_.shutdown(tcp::socket::shutdown_both, ec);
        socket_.close(ec);
    }

    emit disconnected();
    std::cout << "Déconnecté" << std::endl;
}