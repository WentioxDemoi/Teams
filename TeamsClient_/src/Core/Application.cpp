// #include "Application.h"

// Application::Application(int &argc, char **argv) : qtApp(argc, argv)
// {
//   QCoreApplication::setApplicationName("Teams");
//   QFile file(":/qss/styles.qss");
//   if (!file.open(QFile::ReadOnly | QFile::Text)) {
//     qWarning() << "Style QSS introuvable";
//     return;
// }
//   qtApp.setStyleSheet(file.readAll());
// }

// void Application::initializePerms()
// {
//     // QCameraPermission cameraPermission;
//     // auto status = qApp->checkPermission(cameraPermission);
    
//     // if (status == Qt::PermissionStatus::Undetermined) {
//     //     qApp->requestPermission(cameraPermission, [this](const QPermission &permission) {
//     //         if (permission.status() == Qt::PermissionStatus::Granted) {
//     //             qDebug() << "Camera permission granted";
//     //         } else {
//     //             qDebug() << "Camera permission denied";
//     //         }
//     //     });
//     // } else if (status == Qt::PermissionStatus::Granted) {
//     //     qDebug() << "Camera already granted";
//     // } else {
//     //     qDebug() << "Camera permission denied";
//     // }

//     // QMicrophonePermission microphonePermission;
//     // status = qApp->checkPermission(microphonePermission);
    
//     // if (status == Qt::PermissionStatus::Undetermined) {
//     //     qApp->requestPermission(microphonePermission, [this](const QPermission &permission) {
//     //         if (permission.status() == Qt::PermissionStatus::Granted) {
//     //             qDebug() << "Microphone permission granted";
//     //         } else {
//     //             qDebug() << "Microphone permission denied";
//     //         }
//     //     });
//     // } else if (status == Qt::PermissionStatus::Granted) {
//     //     qDebug() << "Microphone already granted";
//     // } else {
//     //     qDebug() << "Microphone permission denied";
//     // }

//     initializeUI();
// }

// void Application::initializeUI()
// {
//   mainWindow = new MainWindow();
//   QObject::connect(mainWindow, &QObject::destroyed, &qtApp, &QCoreApplication::quit);
//   initializeServices();
// }

// void Application::initializeServices()
// {
//   appRoot = new QObject(&qtApp);
//   auto &locator = ServiceLocator::instance();

//   locator.registerService<IUserService>(new UserService(appRoot));
//   locator.registerService<IAuthNetworkService>(new AuthNetworkService(appRoot));

  
//   initializeViewModels();
// }

// void Application::initializeViewModels()
// {
//   auto &locator = ViewModelsLocator::instance();
//   locator.registerViewModels<AuthViewModel>(new AuthViewModel(nullptr, appRoot));
//   initializeViews();
// }

// void Application::initializeViews()
// {
//   auto &locator = ViewLocator::instance();

//   locator.registerView<AuthView>(new AuthView(mainWindow));
//   locator.registerView<WorkspaceView>(new WorkspaceView(mainWindow));
//   locator.registerView<LoadingView>(new LoadingView("Loading your ecosystem", mainWindow));
//   run();
// }

// int Application::run() { 
//   mainWindow->start();
//   mainWindow->show();
//   return qtApp.exec(); 
// }


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
  // initializeUI();
  initializePerms();
}

void Application::initializePerms()
{
    QCameraPermission cameraPermission;
    auto status = qApp->checkPermission(cameraPermission);
    
    if (status == Qt::PermissionStatus::Undetermined) {
        qApp->requestPermission(cameraPermission, [this](const QPermission &permission) {
            if (permission.status() == Qt::PermissionStatus::Granted) {
                qDebug() << "Camera permission granted";
            } else {
                qDebug() << "Camera permission denied";
            }
        });
    } else if (status == Qt::PermissionStatus::Granted) {
        qDebug() << "Camera already granted";
    } else {
        qDebug() << "Camera permission denied";
    }

    // QMicrophonePermission microphonePermission;
    // status = qApp->checkPermission(microphonePermission);
    
    // if (status == Qt::PermissionStatus::Undetermined) {
    //     qApp->requestPermission(microphonePermission, [this](const QPermission &permission) {
    //         if (permission.status() == Qt::PermissionStatus::Granted) {
    //             qDebug() << "Microphone permission granted";
    //         } else {
    //             qDebug() << "Microphone permission denied";
    //         }
    //     });
    // } else if (status == Qt::PermissionStatus::Granted) {
    //     qDebug() << "Microphone already granted";
    // } else {
    //     qDebug() << "Microphone permission denied";
    // }

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

