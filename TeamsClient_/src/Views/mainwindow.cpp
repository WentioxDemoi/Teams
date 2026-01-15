#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Test");
  resize(800, 600);
  authViewModel = new AuthViewModel(this);
  AuthView *authView = new AuthView(this);
  workspaceView = new WorkspaceView(this);

  setCentralWidget(authView);

  connect(authView->loginForm, &LoginForm::loginRequested, authViewModel,
          &AuthViewModel::loginUser);

  connect(authView->registerForm, &RegisterForm::registerRequested,
          authViewModel, &AuthViewModel::registerUser);

  connect(authViewModel, &AuthViewModel::authSuccess, this,
          &MainWindow::authSuccess);

  connect(authViewModel, &AuthViewModel::loginError, this,
          [](const QString &error) { qDebug() << "Error loggin : " << error; });
}

void MainWindow::authSuccess(const User &user) {
  qDebug() << "Switch fenetre";
  setCentralWidget(workspaceView);
}

MainWindow::~MainWindow() {}