#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../Models/User.h"
#include "../ViewModels/AuthViewModel.h"
#include "Views/Auth/AuthView.h"
#include "Views/Workspace/WorkspaceView.h"
#include "Loading/LoadingView.h"
#include "../Core/ViewLocator.h"
#include "../Core/ViewModelsLocator.h"

/**
 * @class MainWindow
 * @brief Fenêtre principale de l'application.
 *
 * Gère l'affichage des différentes vues (AuthView, WorkspaceView) et coordonne
 * l'interaction entre l'interface utilisateur et les ViewModels, notamment
 * la gestion du succès de l'authentification.
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  void start();
  ~MainWindow();

private:
  AuthViewModel *authViewModel;
  WorkspaceView *workspaceView;
  AuthView *authView;
  LoadingView *loadingView;
  QStackedWidget *stack;
  void closeEvent(QCloseEvent *event);

public slots:
  void authSuccess(const User &user);
  void noTokenFound();
};

#endif