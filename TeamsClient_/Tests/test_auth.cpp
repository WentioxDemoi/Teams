#include <gtest/gtest.h>
#include <QApplication>
#include <QTest>
#include "../Organisms/AuthUI.h"
#include "../Molecules/LoginWidget.h"
#include "../Molecules/RegisterWidget.h"

class AuthTest : public ::testing::Test {
protected:
    void SetUp() override {
        auth = new Auth();
    }

    void TearDown() override {
        delete auth;
    }

    Auth *auth;
};

TEST_F(AuthTest, WidgetsAreCreated) {
    EXPECT_NE(auth->findChild<LoginWidget*>(), nullptr);
    EXPECT_NE(auth->findChild<RegisterWidget*>(), nullptr);
    EXPECT_NE(auth->findChild<QStackedWidget*>(), nullptr);
    EXPECT_NE(auth->findChild<QPushButton*>(), nullptr);
}

TEST_F(AuthTest, SwitchPageTogglesStack) {
    QStackedWidget *stack = auth->findChild<QStackedWidget*>();
    ASSERT_NE(stack, nullptr);

    QPushButton *button = auth->findChild<QPushButton*>();
    
    int initialIndex = stack->currentIndex();
    QTest::mouseClick(button, Qt::LeftButton);
    EXPECT_NE(stack->currentIndex(), initialIndex);

    QTest::mouseClick(button, Qt::LeftButton);
    EXPECT_EQ(stack->currentIndex(), initialIndex);
}

TEST_F(AuthTest, SwitchButtonChangesPage) {
    QStackedWidget *stack = auth->findChild<QStackedWidget*>();
    QPushButton *button = auth->findChild<QPushButton*>();
    ASSERT_NE(stack, nullptr);
    ASSERT_NE(button, nullptr);

    int initialIndex = stack->currentIndex();
    QTest::mouseClick(button, Qt::LeftButton);
    EXPECT_NE(stack->currentIndex(), initialIndex);
}