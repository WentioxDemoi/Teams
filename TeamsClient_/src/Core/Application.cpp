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

    auto network = new AuthNetworkService();

    auto authService = new AuthService(network);
    locator.registerService(authService);

    auto authViewModel = new AuthViewModel(authService);
    locator.registerService(authViewModel);
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