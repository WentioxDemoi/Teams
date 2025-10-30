#ifndef AUTHCONNECTION_H
#define AUTHCONNECTION_H

#include <QObject>
#include <QString>
#include <boost/asio.hpp>
#include <atomic>
#include <thread>
#include <QDebug>

class AuthManager : public QObject {
    Q_OBJECT

public:
    explicit AuthManager(boost::asio::io_context& io, const std::string& host, unsigned short port, QObject* parent = nullptr);
    ~AuthManager();

signals:
    void loginSuccess();
    void loginFailed(const QString& reason);
    void registerSuccess();
    void registerFailed(const QString& reason);
    void errorOccurred(const QString& err);

public slots:
    void connectToServer();
    void authenticate(const QString& username, const QString& password, bool isLogin);
    void disconnect();

private:
    void readResponse();

    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::resolver resolver_;
    std::string host_;
    unsigned short port_;
    std::atomic<bool> connected_;
};

#endif