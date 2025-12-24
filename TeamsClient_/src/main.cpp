#include <QApplication>
#include "Core/Application.h"
#include "Views/mainwindow.h"

int main(int argc, char *argv[])
{
    // QApplication app(argc, argv);

    // MainWindow w;
    // w.show();
    Application *app = new Application(argc, argv);

    // return app.exec();
}