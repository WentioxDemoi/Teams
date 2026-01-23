#include "Application.h"

Application::Application(int &argc, char **argv) : qtApp(argc, argv)
{
  QCoreApplication::setApplicationName("Teams");
  initializeServices();
  initializeUI();
  run();
}

void Application::initializeServices()
{
  auto &locator = ServiceLocator::instance();

  locator.registerService<UserService>(new UserService());
  locator.registerService<AuthNetworkService>(new AuthNetworkService());
  locator.registerService<IAuthService>(new AuthService());
  // locator.registerService<UserService>(new UserService());
}

void Application::initializeUI()
{
  MainWindow *mainWindow = new MainWindow();
  mainWindow->show();
}

int Application::run() { return qtApp.exec(); }

