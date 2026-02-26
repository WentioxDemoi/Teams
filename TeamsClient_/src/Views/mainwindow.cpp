#include "mainwindow.h"
#include "WebRTCService.h"
#include "WebRTCViewModel.h"
#include "Workspace/WorkspaceView.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
}

void MainWindow::start()
{
        setWindowTitle("Test");
        resize(800, 600);
        authViewModel = ViewModelsLocator::instance().getViewModels<AuthViewModel>();
        webRTCViewModel = ViewModelsLocator::instance().getViewModels<WebRTCViewModel>();
        authView = ViewLocator::instance().getView<AuthView>();
        workspaceView = ViewLocator::instance().getView<WorkspaceView>();
        loadingView = ViewLocator::instance().getView<LoadingView>();
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

        connect(authViewModel, &AuthViewModel::authError, this,
                [](const QString &error)
                { qDebug() << "Error loggin : " << error; });
        connect(authViewModel, &AuthViewModel::noTokenFound, this,
                &MainWindow::noTokenFound);
        connect(webRTCViewModel, &WebRTCViewModel::onP2PChange, workspaceView, &WorkspaceView::OnP2PChange);
        connect(workspaceView, &WorkspaceView::initP2P, webRTCViewModel, &WebRTCViewModel::initP2P);
        authViewModel->start();
}

void MainWindow::noTokenFound()
{
        qDebug() << "Pas de token";
        sleep(1);
        stack->setCurrentWidget(authView);
}

void MainWindow::authSuccess(const User &user)
{
        qDebug() << "Switch fenetre";
        sleep(1);
        stack->setCurrentWidget(workspaceView);

}



void MainWindow::closeEvent(QCloseEvent *event)
{
        auto *network_ = ServiceLocator::instance().getService<IAuthNetworkService>();
        network_->disconnectFromServer();
        auto *webRTCService_ = ServiceLocator::instance().getService<WebRTCService>();
        webRTCService_->disconnectFromServer();
}

MainWindow::~MainWindow() {}