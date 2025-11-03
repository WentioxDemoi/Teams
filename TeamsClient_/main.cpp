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

    return a.exec();
}