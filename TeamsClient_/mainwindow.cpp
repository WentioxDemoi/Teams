#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Organisms/Auth.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Remplace ton QLabel test par le LoginWidget
    Auth* login = new Auth(this);
    setCentralWidget(login);
}

MainWindow::~MainWindow()
{
    delete ui;
}