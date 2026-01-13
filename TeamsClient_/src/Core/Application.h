#ifndef APPLICATION_H
#define APPLICATION_H

#include "../Views/mainwindow.h"
#include <QApplication>

#include "../Services/AuthService.h"
#include "../ViewModels/AuthViewModel.h"
#include "ServiceLocator.h"

class Application {
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