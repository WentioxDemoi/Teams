#include <QtTest>

#include "ModelTests/UserTest.h"
#include "ViewModelTests/AuthViewModelTest.h"
#include "ServiceTests/AuthNetworkServiceTest.h"



int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    // qRegisterMetaType<User>("User"); // Attention, type User peut être à déclarer dans le futur si opérations plus complexes

    int status = 0;

    {
        UserTest tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    {
        AuthViewModelTest tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    {
        AuthNetworkServiceTest tc;
        status |= QTest::qExec(&tc, argc, argv);
    }
    return status;
}