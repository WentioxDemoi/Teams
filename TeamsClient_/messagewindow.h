#ifndef MESSAGEWINDOW_H
#define MESSAGEWINDOW_H

#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>
#include "message2window.h"
// #include "mainwindow.h"

class MainWindow;


class MessageWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MessageWindow(QStackedWidget *list, QWidget *parent = nullptr, MainWindow *main = nullptr);
    ~MessageWindow();
    void nextWindow();
    void Init();
    void previousWindow();

private:
    QStackedWidget *List_;
    QWidget *previous;
    QWidget *next;
    QPushButton *previousButton_;
    QPushButton *nextButton_;
    MainWindow *main_;
};
#endif // MESSAGEWINDOW_H