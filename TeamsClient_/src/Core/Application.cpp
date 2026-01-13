#include "Application.h"

Application::Application(int& argc, char** argv)
    : qtApp(argc, argv)
{
    initializeServices();
    initializeUI();
    run();
}

void Application::initializeServices() {
    auto& locator = ServiceLocator::instance();

    auto authNetworkService = new AuthNetworkService();
    locator.registerService(authNetworkService);
    auto authService = new AuthService();
    locator.registerService(authService);

    
}

void Application::initializeUI()
{
    MainWindow* mainWindow = new MainWindow();
    mainWindow->show();
}

int Application::run()
{
    return qtApp.exec();
}