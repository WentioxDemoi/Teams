#include "mainwindow.h"
#include "Views/Auth/AuthView.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Qt Minimal App");
    resize(800, 600);
    AuthView *view = new AuthView(this);
    setCentralWidget(view);
}

MainWindow::~MainWindow() {}