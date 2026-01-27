#include "Application.h"

Application::Application(int &argc, char **argv) : qtApp(argc, argv)
{
  QCoreApplication::setApplicationName("Teams");
  initializeUI();
}

void Application::initializeUI()
{
  mainWindow = new MainWindow();
  QObject::connect(mainWindow, &QObject::destroyed, &qtApp, &QCoreApplication::quit);
  initializeServices();
}

void Application::initializeServices()
{
  appRoot = new QObject(&qtApp);
  auto &locator = ServiceLocator::instance();

  locator.registerService<UserService>(new UserService(appRoot));
  locator.registerService<AuthNetworkService>(new AuthNetworkService(appRoot));
  // locator.registerService<IAuthService>(new AuthService(appRoot));

  
  initializeViewModels();
}

void Application::initializeViewModels()
{
  auto &locator = ViewModelsLocator::instance();
  locator.registerViewModels<AuthViewModel>(new AuthViewModel(nullptr, appRoot));
  initializeViews();
}

void Application::initializeViews()
{
  auto &locator = ViewLocator::instance();

  locator.registerView<AuthView>(new AuthView(mainWindow));
  locator.registerView<WorkspaceView>(new WorkspaceView(mainWindow));
  locator.registerView<LoadingView>(new LoadingView("Loading your ecosystem", mainWindow));
  run();
}

int Application::run() { 
  mainWindow->start();
  mainWindow->show();
  return qtApp.exec(); 
}

