#include "mainwindow.h"
#include "Network/NetworkManager.h"
#include <QApplication>
#include <QThread>
#include <QDebug>
#include <QTimer>

void onConnected()
{
    qDebug() << "Connecté au serveur !";
}

void onMessageReceived(const QString& msg)
{
    qDebug() << "Message reçu :" << msg;
}

void onErrorOccurred(const QString& err)
{
    qDebug() << "Erreur réseau :" << err;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // NetworkManager* client = new NetworkManager("127.0.0.1");

    // QThread* networkThread = new QThread;

    // client->moveToThread(networkThread);

    // QObject::connect(networkThread, &QThread::started, client, &NetworkManager::start);
    // QObject::connect(client, &NetworkManager::connected, &w, &onConnected);
    // QObject::connect(client, &NetworkManager::messageReceived, &w, &onMessageReceived);
    // QObject::connect(client, &NetworkManager::errorOccurred, &w, &onErrorOccurred);
    // QObject::connect(client, &NetworkManager::disconnected, networkThread, &QThread::quit);
    // QObject::connect(client, &NetworkManager::disconnected, client, &NetworkManager::deleteLater);
    // QObject::connect(networkThread, &QThread::finished, networkThread, &QThread::deleteLater);

    // networkThread->start();

    // QTimer::singleShot(1000, [client](){
    //     client->sendMessage("Bonjour serveur !\n");
    // });

    return a.exec();
}