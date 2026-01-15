#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../Models/User.h"
#include "../ViewModels/AuthViewModel.h"
#include "Views/Auth/AuthView.h"
#include "Views/Workspace/WorkspaceView.h"
#include <QMainWindow>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:
  AuthViewModel *authViewModel;
  WorkspaceView *workspaceView;

public slots:
  void authSuccess(const User &user);
};

#endif