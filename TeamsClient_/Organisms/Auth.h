#ifndef AUTH_H
#define AUTH_H

#include <QWidget>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QPushButton>

class LoginWidget;
class RegisterWidget;

class Auth : public QWidget {
    Q_OBJECT

public:
    explicit Auth(QWidget* parent = nullptr);
    ~Auth();

private slots:
    void switchPage();

private:
    QVBoxLayout *mainLayout;
    QStackedWidget *stack;
    QPushButton *switchButton;

    LoginWidget *loginWidget;
    RegisterWidget *registerWidget;

    bool showingLogin = true;
};

#endif