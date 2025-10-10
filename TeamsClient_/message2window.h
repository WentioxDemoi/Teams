#ifndef MESSAGE2WINDOW_H
#define MESSAGE2WINDOW_H

#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>

class Message2Window : public QWidget
{
    Q_OBJECT

public:
    explicit Message2Window(QStackedWidget *list, QWidget *parent = nullptr);
    ~Message2Window();
    void PreviousWindow();

private:
    QStackedWidget *List_;
    QPushButton *PushButton_;
    QWidget *next;
    QWidget *prev;
};
#endif // MESSAGE2WINDOW_H