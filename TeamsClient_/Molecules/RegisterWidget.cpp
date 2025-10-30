#include "RegisterWidget.h"
#include "../Organisms/Auth.h"
#include <QLabel>
#include <QDebug>

RegisterWidget::RegisterWidget(Auth *parent)
    : QWidget(parent),
      authParent(parent)
{
    init();
}

bool RegisterWidget::init()
{
    try
    {
        layout = new QVBoxLayout(this);
        layout->setSpacing(10);

        QLabel *title = new QLabel("Inscription");
        title->setStyleSheet("font-size: 22px; font-weight: bold; color: #333;");
        title->setAlignment(Qt::AlignCenter);

        emailField = new InputField("Adresse e-mail");
        passwordField = new InputField("Mot de passe", nullptr, std::nullopt, true);
        confirmPasswordField = new InputField("Confirmer le mot de passe", nullptr, std::nullopt, true);

        registerButton = new Button("Créer un compte", this);
        connect(registerButton, &QPushButton::clicked, this, &RegisterWidget::onRegisterButtonClicked);

        layout->addWidget(title);
        layout->addWidget(emailField);
        layout->addWidget(passwordField);
        layout->addWidget(confirmPasswordField);
        layout->addWidget(registerButton);

        setLayout(layout);

        return true;
    }
    catch (std::exception &ex)
    {
        qDebug() << ex.what();
        return false;
    }
}

void RegisterWidget::onRegisterButtonClicked()
{
    if (emailField->text().isEmpty() || passwordField->text().isEmpty() || confirmPasswordField->text().isEmpty())
    {
        qDebug() << "Veuillez remplir tous les champs.";
    }
    else if (passwordField->text() != confirmPasswordField->text())
    {
        qDebug() << "Les mots de passe ne correspondent pas.";
    }
    else
    {
        qDebug() << "Création de compte pour :" << emailField->text();
        emit registerRequest(emailField->text(), passwordField->text(), false);
    }
}