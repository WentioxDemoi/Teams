#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include "../Views/mainwindow.h"

#include "ServiceLocator.h"
#include "../Services/NetworkService.h"
#include "../Services/AuthService.h"
#include "../ViewModels/AuthViewModel.h"


class Application {
    public :
        Application(int &argc, char **argv);
        int run();

    private :
        QApplication qtApp;

        void initializeServices();
        void initializeUI();
};

#endif