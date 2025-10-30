#include "NetworkManager.h"
#include <iostream>
#include <QThread>

NetworkManager::NetworkManager(const std::string &host, unsigned short port, QObject *parent)
    : QObject(parent), host_(host), port_(port)
{
    authManager_ = std::make_unique<AuthManager>(io_context_, host, port, this);
    // messageManager_ = std::make_unique<MessageManager>(io_context_, host, port, this);

    connect(authManager_.get(), &AuthManager::loginSuccess, this, &NetworkManager::loginSuccess);
    connect(authManager_.get(), &AuthManager::loginFailed, this, &NetworkManager::loginFailed);
    connect(authManager_.get(), &AuthManager::errorOccurred, this, &NetworkManager::errorOccurred);

    // connect(messageManager_.get(), &MessageManager::messageReceived, this, &NetworkManager::messageReceived);
    // connect(messageManager_.get(), &MessageManager::errorOccurred, this, &NetworkManager::errorOccurred);
}

NetworkManager::~NetworkManager()
{
    disconnect();
}

void NetworkManager::connectToServer()
{
    try
    {
        authManager_->connectToServer();
        // messageManager_->connectToServer();
        emit connected();
    }
    catch (const std::exception &e)
    {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

void NetworkManager::authenticate(const QString &username, const QString &password, bool isLogin)
{
    authManager_->authenticate(username, password, isLogin);
}

void NetworkManager::sendChatMessage(const QString &msg)
{
    // messageManager_->sendMessage(msg);
}

void NetworkManager::disconnect()
{
    authManager_->disconnect();
    // messageManager_->disconnect();
    emit disconnected();
}