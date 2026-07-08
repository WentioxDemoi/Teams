#ifndef APPLICATION_H
#define APPLICATION_H

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QObject>

/**
 * @class Application
 * @brief Point d'entrée principal de l'application.
 *
 * Initialise l'environnement Qt, les services, les ViewModels et l'interface utilisateur,
 * puis lance la boucle principale de l'application.
 */
class Application {
public:
    Application(int& argc, char** argv);
    ~Application() = default;
    int run();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

private:
    QGuiApplication qtApp;      // QApplication → QGuiApplication
    QQmlApplicationEngine engine;
    QObject* appRoot = nullptr;

    void initializePerms();
    void initializeServices();
    void initializeViewModels();
    void initializeModels();
    // void connectViewModels();
};

#endif