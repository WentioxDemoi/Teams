#include "InputField.h"

InputField::InputField(const QString& placeholder, QWidget* parent, std::optional<QSize> size, bool isPassword)
    : QLineEdit(parent)
{
    // Placeholder
    setPlaceholderText(placeholder);

    // Taille optionnelle
    if (size.has_value())
        setFixedSize(size.value());
    else
        setFixedHeight(35); // hauteur standard

    // Style par défaut
    setStyleSheet(
        "QLineEdit {"
        " border: 1px solid #ccc;"
        " border-radius: 5px;"
        " padding: 6px 10px;"
        " font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        " border: 1px solid #0078d7;"
        "}"
    );

    // Champ mot de passe
    if (isPassword)
        setEchoMode(QLineEdit::Password);
}