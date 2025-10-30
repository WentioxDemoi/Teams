#ifndef AUTH_H
#define AUTH_H

#include <QWidget>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QDebug>

class LoginWidget;
class RegisterWidget;

class Auth : public QWidget {
    Q_OBJECT

public:
    explicit Auth(QWidget* parent = nullptr);
    ~Auth();

private slots:
    void switchPage();

public slots:
    void handleLoginRequest(const QString& email, const QString& password, bool isLogin);

signals:
    void loginRequest(const QString &email, const QString &password, bool isLogin);
    void registerRequest(const QString &email, const QString &password, bool isLogin);

private:
    QVBoxLayout *mainLayout;
    QStackedWidget *stack;
    QPushButton *switchButton;

    LoginWidget *loginWidget;
    RegisterWidget *registerWidget;

    bool showingLogin = true;
};

#endif