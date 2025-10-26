#include <gtest/gtest.h>
#include <QApplication>
#include <QTest>
#include "../Atoms/InputField.h"

TEST(InputFieldTest, PlaceholderTextIsSet)
{
    InputField field("Entrez votre nom");
    EXPECT_EQ(field.placeholderText().toStdString(), "Entrez votre nom");
}

TEST(InputFieldTest, PasswordModeIsApplied)
{
    InputField passwordField("", nullptr, std::nullopt, true);
    EXPECT_EQ(passwordField.echoMode(), QLineEdit::Password);
}

TEST(InputFieldTest, NormalModeByDefault)
{
    InputField normalField;
    EXPECT_EQ(normalField.echoMode(), QLineEdit::Normal);
}

TEST(InputFieldTest, SizeOptionIsApplied)
{
    QSize expectedSize(200, 40);
    InputField field("", nullptr, expectedSize);
    EXPECT_EQ(field.size(), expectedSize);
}

TEST(InputFieldTest, UserInputIsStored)
{
    InputField field;
    QTest::keyClicks(&field, "Hello");
    EXPECT_EQ(field.text().toStdString(), "Hello");
}