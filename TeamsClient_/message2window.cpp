#include "message2window.h"


Message2Window::Message2Window(QStackedWidget *list, QWidget *parent)
    // : QWidget(parent)
{
    prev = parent;
    List_ = list;
    QVBoxLayout *box = new QVBoxLayout(this);
    QLabel *message = new QLabel("Message 2", this);
    QPushButton *previous = new QPushButton("Previous window", this);
    message->setAlignment(Qt::AlignCenter);
    box->addWidget(message);
    box->addWidget(previous);
    // setLayout(box);
    setWindowTitle("Message fenetre 2");
    connect(previous, &QPushButton::clicked, this, &Message2Window::PreviousWindow);
}

void Message2Window::PreviousWindow()
{
    List_->setCurrentWidget(prev);
}

Message2Window::~Message2Window()
{
}