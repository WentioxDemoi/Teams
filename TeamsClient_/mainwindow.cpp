#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Network/NetworkManager.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      networkManager_(nullptr)
{
    ui->setupUi(this);

    authUI_ = new Auth(this);
    setCentralWidget(authUI_);
    initializeNetwork();
    setupConnections();
    networkManager_->connectToServer();
}

MainWindow::~MainWindow()
{
    delete networkManager_;
    delete ui;
}

void MainWindow::initializeNetwork()
{
    const std::string host = "127.0.0.1";
    const unsigned short port = 12345;

    networkManager_ = new NetworkManager(host, port, this);
}

void MainWindow::setupConnections()
{
    connect(networkManager_, &NetworkManager::connected, this, &MainWindow::onConnected);
    connect(networkManager_, &NetworkManager::disconnected, this, &MainWindow::onDisconnected);
    connect(networkManager_, &NetworkManager::errorOccurred, this, &MainWindow::onErrorOccurred);

    connect(networkManager_, &NetworkManager::loginSuccess, this, &MainWindow::onLoginSuccess);
    connect(networkManager_, &NetworkManager::loginFailed, this, &MainWindow::onLoginFailed);
    connect(networkManager_, &NetworkManager::messageReceived, this, &MainWindow::onMessageReceived);

    connect(authUI_, &Auth::loginRequest, networkManager_, &NetworkManager::authenticate);
}

void MainWindow::onConnected()
{
    qDebug() << "[MainWindow] Connected to server!";
}

void MainWindow::onDisconnected()
{
    qDebug() << "[MainWindow] Disconnected from server.";
}

void MainWindow::onErrorOccurred(const QString& err)
{
    qWarning() << "[MainWindow] Network error:" << err;
}

void MainWindow::onLoginSuccess()
{
    qDebug() << "[MainWindow] Login successful!";
}

void MainWindow::onLoginFailed(const QString& reason)
{
    qDebug() << "[MainWindow] Login failed:" << reason;
}

void MainWindow::onMessageReceived(const QString& msg)
{
    qDebug() << "[MainWindow] Message received:" << msg;
}

// void MainWindow::attemptLogin(const QString& username, const QString& password)
// {
//     if (!networkManager_)
//     {
//         qWarning() << "NetworkManager not initialized!";
//         return;
//     }

//     networkManager_->authenticate(username, password);
// }