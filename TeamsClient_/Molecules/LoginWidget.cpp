#include "LoginWidget.h"
#include "../Organisms/Auth.h"
#include <QLabel>
#include <QDebug>

LoginWidget::LoginWidget(Auth *parent)
    : QWidget(parent),
      authParent(parent)
{
    init();
}

bool LoginWidget::init()
{
    try
    {

        layout = new QVBoxLayout(this);
        layout->setSpacing(10);

        QLabel *title = new QLabel("Connexion");
        title->setStyleSheet("font-size: 22px; font-weight: bold; color: #333;");
        title->setAlignment(Qt::AlignCenter);

        emailField = new InputField("Adresse e-mail");
        passwordField = new InputField("Mot de passe", nullptr, std::nullopt, true);

        loginButton = new Button("Se connecter", this);
        connect(loginButton, &Button::clicked, this, &LoginWidget::onLoginButtonClicked);

        layout->addWidget(title);
        layout->addWidget(emailField);
        layout->addWidget(passwordField);
        layout->addWidget(loginButton);

        setLayout(layout);
        return true;
    }
    catch (std::exception &ex)
    {
        qDebug() << ex.what();
        return false;
    }
}

void LoginWidget::onLoginButtonClicked()
{
    if (emailField->text().isEmpty() || passwordField->text().isEmpty())
    {
        qDebug() << "Veuillez remplir tous les champs.";
    }
    else
    {
        qDebug() << "Tentative de connexion avec :" << emailField->text() << passwordField->text();
        emit loginRequest(emailField->text(), passwordField->text(), true);
    }
}