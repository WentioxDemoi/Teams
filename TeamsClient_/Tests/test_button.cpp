#include <gtest/gtest.h>
#include <QApplication>
#include <QSignalSpy>
#include <QTest>
#include "../Atoms/Button.h"

TEST(ButtonTest, DefaultConstructorSetsText)
{
    Button btn;
    EXPECT_EQ(btn.text().toStdString(), "Bouton");
}

TEST(ButtonTest, CustomTextIsApplied)
{
    Button btn("Se connecter");
    EXPECT_EQ(btn.text().toStdString(), "Se connecter");
}

TEST(ButtonTest, OnClickHandlerIsCalled)
{
    bool clicked = false;

    Button btn("Test");
    btn.setOnClick([&]() { clicked = true; });

    QTest::mouseClick(&btn, Qt::LeftButton);

    EXPECT_TRUE(clicked);
}

TEST(ButtonTest, SizeAndColorOptionsAreApplied)
{
    QSize expectedSize(100, 50);
    QString expectedColor = "red";

    Button btn("Test", nullptr, expectedSize, expectedColor);

    EXPECT_EQ(btn.size(), expectedSize);
}