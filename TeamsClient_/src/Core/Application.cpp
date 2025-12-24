#include "Application.h"

Application::Application(int& argc, char** argv)
    : qtApp(argc, argv)
{
    initializeServices();
    initializeUI();
    run();
}

void Application::initializeServices()
{
    // auto networkService = new NetworkService();
    auto authService = new AuthService(networkService);
    auto authViewModel = new AuthViewModel(authService);

    auto& locator = ServiceLocator::instance();
    // locator.registerService(networkService);
    locator.registerService(authService);
    locator.registerService(authViewModel);
}

void Application::initializeUI()
{
    // Création et affichage de la fenêtre principale
    MainWindow* mainWindow = new MainWindow();
    mainWindow->show();
}

int Application::run()
{
    return qtApp.exec();
}