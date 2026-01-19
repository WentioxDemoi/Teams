#include "mainwindow.h"
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
        setWindowTitle("Test");
        resize(800, 600);
        authViewModel = new AuthViewModel(nullptr, this);
        authView = new AuthView(this);
        workspaceView = new WorkspaceView(this);
        loadingView = new LoadingDialog("Loading your ecosystem", this);
        stack = new QStackedWidget(this);
        stack->addWidget(authView);
        stack->addWidget(workspaceView);
        stack->addWidget(loadingView);

        setCentralWidget(stack);
        stack->setCurrentWidget(loadingView);

        connect(authView->loginForm, &LoginForm::loginRequested, authViewModel,
                &AuthViewModel::loginUser);

        connect(authView->registerForm, &RegisterForm::registerRequested,
                authViewModel, &AuthViewModel::registerUser);

        connect(authViewModel, &AuthViewModel::authSuccess, this,
                &MainWindow::authSuccess);

        connect(authViewModel, &AuthViewModel::loginError, this,
                [](const QString &error)
                { qDebug() << "Error loggin : " << error; });
        connect(authViewModel, &AuthViewModel::noTokenFound, this,
                &MainWindow::noTokenFound);
}

void MainWindow::noTokenFound()
{
        sleep(1);
        stack->setCurrentWidget(authView);
}

void MainWindow::authSuccess(const User &user)
{
        qDebug() << "Switch fenetre";
        stack->setCurrentWidget(workspaceView);
}

MainWindow::~MainWindow() {}