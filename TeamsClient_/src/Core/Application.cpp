#include "Application.h"

#include <QtCore/qpermissions.h>

#include <QDebug>
#include <QFile>
#include <QQmlContext>

#include "../Utils/Interfaces/ITokenManager.h"
#include "../Utils/TokenManager.h"
#include "../ViewModels/AuthViewModel.h"
#include "../ViewModels/ChatViewModel.h"
#include "Auth/AuthService.h"
#include "Call/CallService.h"
#include "ChatService.h"
#include "Contact/ContactService.h"
#include "Interfaces/ICallService.h"
#include "Interfaces/IChatService.h"
#include "Interfaces/IAuthService.h"
#include "Interfaces/IContactService.h"
#include "Interfaces/IMessageService.h"
#include "Message/MessageService.h"
#include "MessageList.h"
#include "ModelLocator.h"
#include "ServiceLocator.h"
#include "State/SessionState.h"
#include "State/UserState.h"
#include "StateLocator.h"
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

  auto& serviceLocator = ServiceLocator::instance();
  auto& stateLocator = StateLocator::instance();

  stateLocator.registerState<UserState>(&UserState::instance());
  stateLocator.registerState<SessionState>(&SessionState::instance());

  serviceLocator.registerService<ITokenManager>(&TokenManager::instance());

  auto* userService = new UserService(nullptr, nullptr, appRoot);
  serviceLocator.registerService<IUserService>(userService);

  auto* messageService = new MessageService(nullptr, nullptr, appRoot);
  serviceLocator.registerService<IMessageService>(messageService);

  auto* contactService = new ContactService(nullptr, nullptr, appRoot);
  serviceLocator.registerService<IContactService>(contactService);

  auto* callService = new CallService(nullptr, appRoot);
  serviceLocator.registerService<ICallService>(callService);

  auto* chatService = new ChatService(nullptr, nullptr, nullptr, appRoot);
  serviceLocator.registerService<IChatService>(chatService);

  // AuthService EN DERNIER
  auto* authService =
      new AuthService(nullptr, nullptr, nullptr, nullptr, appRoot);

  serviceLocator.registerService<IAuthService>(authService);

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

  auto* authVM = new AuthViewModel(nullptr, nullptr, nullptr, appRoot);

  locator.registerViewModels<AuthViewModel>(authVM);

  auto* chatVM = new ChatViewModel(nullptr, nullptr, appRoot);

  locator.registerViewModels<ChatViewModel>(chatVM);

  engine.rootContext()->setContextProperty("authVM", authVM);
  engine.rootContext()->setContextProperty("chatVM", chatVM);

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &qtApp,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  qRegisterMetaType<User*>("User*");

  engine.loadFromModule("TeamsClient", "Main");
}

int Application::run() { return qtApp.exec(); }
