#ifndef APPLICATION_H
#define APPLICATION_H

#include "../Views/mainwindow.h"
#include "../Services/AuthService.h"
#include "../ViewModels/AuthViewModel.h"
#include "ServiceLocator.h"
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

  void initializeServices();
  void initializeViewModels();
  void initializeUI();
};

#endif