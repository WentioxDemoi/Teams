#include <QtTest>
#include <QCoreApplication>

#include "Models/Message_test.h"
#include "Models/User_test.h"





int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    UserTest userTest;
    MessageTest messageTest;

    int status = QTest::qExec(&userTest, argc, argv);
    status |= QTest::qExec(&messageTest, argc, argv);
    return status;
}