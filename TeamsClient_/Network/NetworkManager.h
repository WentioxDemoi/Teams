#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QString>
#include <boost/asio.hpp>
#include <thread>
#include <atomic>

class NetworkManager : public QObject {
    Q_OBJECT

public:
    explicit NetworkManager(const std::string& host, QObject *parent = nullptr);
    ~NetworkManager();

signals:
    void connected();
    void disconnected();
    void messageReceived(const QString& msg);
    void errorOccurred(const QString& err);

public slots:
    void start();
    void sendMessage(const QString& message);
    void stop();

private:
    void run();

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::socket socket_;
    std::string host_;
    static constexpr unsigned short port_ = 12345;

    std::atomic<bool> running_;
};

#endif