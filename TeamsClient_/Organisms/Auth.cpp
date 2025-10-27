#include "Auth.h"
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
    // switchButton->setStyleSheet("QPushButton { font-weight: bold; color: #0078d4; border: none; background: transparent; }"
    //                             "QPushButton:hover { text-decoration: underline; }");
    connect(switchButton, &QPushButton::clicked, this, &Auth::switchPage);

    topBar->addWidget(switchButton);
    mainLayout->addLayout(topBar);

    stack = new QStackedWidget(this);
    loginWidget = new LoginWidget(this);
    registerWidget = new RegisterWidget(this);

    stack->addWidget(loginWidget);
    stack->addWidget(registerWidget);

    mainLayout->addWidget(stack);
    setLayout(mainLayout);
}

Auth::~Auth() {}

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