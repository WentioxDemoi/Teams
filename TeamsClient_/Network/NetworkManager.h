#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QString>
#include <boost/asio.hpp>
#include <memory>

#include "AuthManager.h"
// #include "MessageManager.h"

class NetworkManager : public QObject {
    Q_OBJECT

public:
    explicit NetworkManager(const std::string& host, unsigned short port, QObject *parent = nullptr);
    ~NetworkManager();

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString& err);

    void authSuccess();
    void authFailed(const QString& reason);
    void messageReceived(const QString& msg);

public slots:
    void connectToServer();
    void authenticate(const QString& username, const QString& password, bool isLogin);
    void sendChatMessage(const QString& msg);
    void disconnect();

private:
    boost::asio::io_context io_context_;
    std::string host_;
    unsigned short port_;

    std::unique_ptr<AuthManager> authManager_;
    // std::unique_ptr<MessageManager> messageManager_;
};

#endif