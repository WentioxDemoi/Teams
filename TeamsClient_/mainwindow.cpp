#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QLabel *test = new QLabel("test");
    setCentralWidget(test);
}

MainWindow::~MainWindow()
{
    delete ui;
}
