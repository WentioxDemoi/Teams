#include <QtTest>
#include <QCoreApplication>

#include "Database/DatabaseManager_test.h"
#include "Repositories/MessageRepository_test.h"
#include "Repositories/UserRepository_test.h"
#include "Core/Locator_test.h"
#include "Models/ContactList_test.h"
#include "Models/Message_test.h"
#include "Models/MessageList_test.h"
#include "Models/SearchResults_test.h"
#include "Models/User_test.h"
#include "Services/ContactService_test.h"
#include "Utils/FrameConverter_test.h"
#include "Services/AuthService_test.h"
#include "Services/CallService_test.h"
#include "Services/ChatService_test.h"
#include "Services/MessageService_test.h"
#include "Services/LocalUserService_test.h"
#include "Services/NetworkService_test.h"
#include "Services/P2PTest.h"
#include "ViewModels/AuthViewModel_test.h"
#include "ViewModels/ChatViewModel_test.h"
#include "ViewModels/WebRTCViewModel_test.h"





int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    ContactListTest contactListTest;
    UserTest userTest;
    MessageTest messageTest;
    MessageListTest messageListTest;
    SearchResultsTest searchResultsTest;
    FrameConverterTest frameConverterTest;
    AuthServiceTest authServiceTest;
    CallServiceTest callServiceTest;
    ChatServiceTest chatServiceTest;
    MessageServiceTest messageServiceTest;
    ContactServiceTest contactServiceTest;
    LocalUserServiceTest localUserServiceTest;
    NetworkServiceTest networkServiceTest;
    LocatorTest locatorTest;
    DatabaseManagerTest databaseManagerTest;
    P2PTest p2pTest;
    MessageRepositoryTest messageRepositoryTest;
    UserRepositoryTest userRepositoryTest;
    AuthViewModelTest authViewModelTest;
    ChatViewModelTest chatViewModelTest;
    WebRTCViewModelTest webRTCViewModelTest;

    int status = QTest::qExec(&contactListTest, argc, argv);
    status |= QTest::qExec(&userTest, argc, argv);
    status |= QTest::qExec(&messageTest, argc, argv);
    status |= QTest::qExec(&messageListTest, argc, argv);
    status |= QTest::qExec(&searchResultsTest, argc, argv);
    status |= QTest::qExec(&frameConverterTest, argc, argv);
    status |= QTest::qExec(&authServiceTest, argc, argv);
    status |= QTest::qExec(&callServiceTest, argc, argv);
    status |= QTest::qExec(&chatServiceTest, argc, argv);
    status |= QTest::qExec(&messageServiceTest, argc, argv);
    status |= QTest::qExec(&contactServiceTest, argc, argv);
    status |= QTest::qExec(&localUserServiceTest, argc, argv);
    status |= QTest::qExec(&networkServiceTest, argc, argv);
    status |= QTest::qExec(&p2pTest, argc, argv);
    status |= QTest::qExec(&locatorTest, argc, argv);
    status |= QTest::qExec(&databaseManagerTest, argc, argv);
    status |= QTest::qExec(&messageRepositoryTest, argc, argv);
    status |= QTest::qExec(&userRepositoryTest, argc, argv);
    status |= QTest::qExec(&authViewModelTest, argc, argv);
    status |= QTest::qExec(&chatViewModelTest, argc, argv);
    status |= QTest::qExec(&webRTCViewModelTest, argc, argv);
    return status;
}