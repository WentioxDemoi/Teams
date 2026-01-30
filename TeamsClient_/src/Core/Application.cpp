#include "Application.h"

Application::Application(int &argc, char **argv) : qtApp(argc, argv)
{
  QCoreApplication::setApplicationName("Teams");
  QFile file(":/qss/styles.qss");
  // file.open(QFile::ReadOnly);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    qWarning() << "Style QSS introuvable";
    return;
}
  qtApp.setStyleSheet(file.readAll());
  // qtApp.setStyleSheet(R"(
  //       QWidget {
  //           background-color: #0f172a;
  //           color: #e5e7eb;
  //           font-family: "Segoe UI";
  //           font-size: 14px;
  //       }

  //       QLineEdit {
  //           background: #020617;
  //           border: 1px solid #334155;
  //           border-radius: 8px;
  //           padding: 10px;
  //       }

  //       QLineEdit:focus {
  //           border: 1px solid #38bdf8;
  //       }

  //       QPushButton {
  //           background: #38bdf8;
  //           color: #020617;
  //           border-radius: 8px;
  //           padding: 10px;
  //           font-weight: bold;
  //       }

  //       QPushButton:hover {
  //           background: #0ea5e9;
  //       }
  //   )");
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

