#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "Network/NetworkManager.h"
#include "Organisms/Auth.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(const QString& err);
    void onLoginSuccess();
    void onLoginFailed(const QString& reason);
    void onMessageReceived(const QString& msg);

private:
    void setupConnections();
    void initializeNetwork();
    void attemptLogin(const QString& username, const QString& password);

private:
    Ui::MainWindow *ui;
    Auth *authUI_;
    NetworkManager *networkManager_;
};

#endif 



//===================== TUTO =====================
//
//  connect(sender, signal, receiver, slot);
//
//================================================