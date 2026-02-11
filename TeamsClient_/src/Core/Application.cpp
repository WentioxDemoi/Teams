#include "Application.h"

Application::Application(int &argc, char **argv) : qtApp(argc, argv)
{
  QCoreApplication::setApplicationName("Teams");
  QFile file(":/qss/styles.qss");
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    qWarning() << "Style QSS introuvable";
    return;
}
  qtApp.setStyleSheet(file.readAll());
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

  locator.registerService<IUserService>(new UserService(appRoot));
  locator.registerService<IAuthNetworkService>(new AuthNetworkService(appRoot));

  
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

