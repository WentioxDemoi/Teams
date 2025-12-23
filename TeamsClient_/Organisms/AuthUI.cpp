#include "AuthUI.h"
#include "../Molecules/LoginWidget.h"
#include "../Molecules/RegisterWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

Auth::Auth(QWidget *parent)
    : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(50, 50, 50, 50);

    QHBoxLayout *topBar = new QHBoxLayout();
    topBar->addStretch();

    switchButton = new QPushButton("S'inscrire", this);
    connect(switchButton, &QPushButton::clicked, this, &Auth::switchPage);

    loginWidget = new LoginWidget(this);
    registerWidget = new RegisterWidget(this);

    connect(loginWidget, &LoginWidget::loginRequest, this, &Auth::handleAuthRequest);
    connect(registerWidget, &RegisterWidget::registerRequest, this, &Auth::handleAuthRequest);

    topBar->addWidget(switchButton);
    mainLayout->addLayout(topBar);

    stack = new QStackedWidget(this);
    stack->addWidget(loginWidget);
    stack->addWidget(registerWidget);
    stack->setCurrentWidget(loginWidget);

    mainLayout->addWidget(stack);
    setLayout(mainLayout);
}

Auth::~Auth() {}

void Auth::handleAuthRequest(const QString& email, const QString& password, bool isLogin)
{
    qDebug() << "Auth received request";
    emit authRequest(email, password, isLogin);
}

void Auth::switchPage()
{
    showingLogin = !showingLogin;

    if (showingLogin) {
        stack->setCurrentWidget(loginWidget);
        switchButton->setText("S'inscrire");
        qDebug() << "Switched to Login page.";
    } else {
        stack->setCurrentWidget(registerWidget);
        switchButton->setText("Se connecter");
        qDebug() << "Switched to Register page.";
    }
}