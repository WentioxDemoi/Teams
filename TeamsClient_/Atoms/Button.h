#ifndef BUTTON_H
#define BUTTON_H

#include <QPushButton>
#include <QString>
#include <functional>
#include <optional>  // pour std::optional

class Button : public QPushButton
{
    Q_OBJECT

public:
    explicit Button(
        const QString& text = "Bouton",
        QWidget* parent = nullptr,
        std::optional<QSize> size = std::nullopt,
        std::optional<QString> color = std::nullopt,
        std::function<void()> onClick = nullptr
    );

    void setOnClick(std::function<void()> func);

private:
    std::function<void()> onClickHandler;

private slots:
    void handleClick();
};

#endif // BUTTON_H