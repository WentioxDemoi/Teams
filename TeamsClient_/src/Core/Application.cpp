#include "Application.h"

#include <QtCore/qpermissions.h>

#include <QDebug>
#include <QFile>
#include <QQmlContext>

#include "../ViewModels/AuthViewModel.h"
#include "../ViewModels/ChatViewModel.h"
#include "Auth/AuthService.h"
#include "Interfaces/IAuthService.h"
#include "Interfaces/ISessionService.h"
#include "MessageList.h"
#include "ModelLocator.h"
#include "ServiceLocator.h"
#include "SessionService.h"
#include "UserList.h"
#include "UserService.h"
#include "ViewModelsLocator.h"

Application::Application(int& argc, char** argv) : qtApp(argc, argv) {
  qputenv("QT_QUICK_CONTROLS_STYLE", "Basic");
  QCoreApplication::setApplicationName("Teams");

  initializePerms();
}

void Application::initializePerms() {
  QCameraPermission cameraPermission;
  auto status = qApp->checkPermission(cameraPermission);

  if (status == Qt::PermissionStatus::Undetermined) {
    qApp->requestPermission(cameraPermission, [this](const QPermission& permission) {
      if (permission.status() == Qt::PermissionStatus::Granted)
        qDebug() << "Camera permission granted";
      else
        qDebug() << "Camera permission denied";
      initializeServices();
    });
  } else {
    qDebug() << (status == Qt::PermissionStatus::Granted ? "Camera already granted"
                                                         : "Camera permission denied");
    initializeServices();
  }
}

void Application::initializeServices() {
  appRoot = new QObject(&qtApp);
  auto& locator = ServiceLocator::instance();

  locator.registerService<IUserService>(new UserService(nullptr, appRoot));
  locator.registerService<IAuthService>(new AuthService(nullptr, appRoot));
  locator.registerService<ISessionService>(new SessionService(nullptr, nullptr, nullptr, appRoot));

  initializeModels();
}

void Application::initializeModels() {
  auto& locator = ModelLocator::instance();

  auto* userListModel = new UserList(appRoot);

  locator.registerModel<UserList>(userListModel);

  auto* messageListModel = new MessageList(appRoot);

  locator.registerModel<MessageList>(messageListModel);

  initializeViewModels();
}

void Application::initializeViewModels() {
  auto& locator = ViewModelsLocator::instance();

  auto* authVM = new AuthViewModel(nullptr, appRoot);

  locator.registerViewModels<AuthViewModel>(authVM);

  auto* chatVM = new ChatViewModel(nullptr, appRoot);

  engine.rootContext()->setContextProperty("authVM", authVM);
  engine.rootContext()->setContextProperty("chatVM", chatVM);

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &qtApp,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  qRegisterMetaType<User*>("User*");

  engine.loadFromModule("TeamsClient", "Main");
}

int Application::run() { return qtApp.exec(); }
