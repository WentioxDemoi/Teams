#ifndef APPLICATION_H
#define APPLICATION_H

// Services
#include "../Services/AuthService.h"

// Views
#include "../Views/mainwindow.h"

// ViewModels
#include "../ViewModels/AuthViewModel.h"

// Locators
#include "ServiceLocator.h"
#include "ViewModelsLocator.h"
#include "ViewLocator.h"


#include "../includes.h"

/**
 * @class Application
 * @brief Point d'entrée principal de l'application.
 *
 * Initialise l'environnement Qt, les services, les ViewModels et l'interface utilisateur,
 * puis lance la boucle principale de l'application.
 */
class Application
{
public:
  Application(int &argc, char **argv);
  ~Application() = default;
  int run();

  Application(const Application &) = delete;
  Application &operator=(const Application &) = delete;

private:
  QApplication qtApp;
  QObject *appRoot;
  MainWindow *mainWindow;

  void initializeServices();
  void initializeViews();
  void initializeViewModels();
  void initializeUI();
};

#endif