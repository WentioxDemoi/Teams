#ifndef INPUTFIELD_H
#define INPUTFIELD_H

#include <QLineEdit>
#include <QString>
#include <optional>

class InputField : public QLineEdit
{
    Q_OBJECT

public:
    explicit InputField(
        const QString& placeholder = "",
        QWidget* parent = nullptr,
        std::optional<QSize> size = std::nullopt,
        bool isPassword = false
    );
};

#endif // INPUTFIELD_H