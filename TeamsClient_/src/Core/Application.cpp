#include "Application.h"

#include "Interfaces/IAuthService.h"
#include "Interfaces/ISessionService.h"
#include "Auth/AuthService.h"
#include "SessionService.h"
#include "UserService.h"

#include "../ViewModels/AuthViewModel.h"

#include "ServiceLocator.h"
#include "ViewModelsLocator.h"

#include <QtCore/qpermissions.h>
#include <QDebug>
#include <QFile>

#include <QQmlContext>

Application::Application(int& argc, char** argv) : qtApp(argc, argv) {
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
        qDebug() << (status == Qt::PermissionStatus::Granted
                     ? "Camera already granted" : "Camera permission denied");
        initializeServices();
    }
}

void Application::initializeServices() {
    appRoot = new QObject(&qtApp);
    auto& locator = ServiceLocator::instance();

    locator.registerService<IUserService>(new UserService(appRoot));
    locator.registerService<IAuthService>(new AuthService(nullptr, nullptr, nullptr, appRoot));
    locator.registerService<ISessionService>(new SessionService(nullptr, appRoot));

    initializeViewModels();
}

void Application::initializeViewModels() {
    auto& locator = ViewModelsLocator::instance();

    auto* authVM   = new AuthViewModel(nullptr, appRoot);

    locator.registerViewModels<AuthViewModel>(authVM);

    engine.rootContext()->setContextProperty("authVM", authVM);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &qtApp, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);

    engine.loadFromModule("TeamsClient", "Main");
}

int Application::run() {
    return qtApp.exec();
}
