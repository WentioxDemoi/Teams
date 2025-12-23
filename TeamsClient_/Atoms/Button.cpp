#include "Button.h"
#include <QDebug>

Button::Button(const QString& text,
               QWidget* parent,
               std::optional<QSize> size,
               std::optional<QString> color,
               std::function<void()> onClick)
    : QPushButton(text, parent), onClickHandler(onClick)
{
    // Valeurs par défaut si non fournies
    QSize finalSize = size.value_or(QSize(120, 40));
    QString finalColor = color.value_or("#0078d7");

    setFixedSize(finalSize);

    setStyleSheet(QString(
        "QPushButton {"
        " background-color: %1;"
        " color: white;"
        " border: none;"
        " border-radius: 5px;"
        " font-weight: bold;"
        "}"
        "QPushButton:hover {"
        " background-color: #005a9e;"
        "}"
    ).arg(finalColor));

    connect(this, &QPushButton::clicked, this, &Button::handleClick);
}

void Button::setOnClick(std::function<void()> func)
{
    onClickHandler = func;
}

void Button::handleClick()
{
    if (onClickHandler)
        onClickHandler();
    else
        qDebug() << "Aucun callback défini pour ce bouton.";
}