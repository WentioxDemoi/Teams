#include "messagewindow.h"
#include "mainwindow.h"


MessageWindow::MessageWindow(QStackedWidget *list, QWidget *parent, MainWindow *main)
    : QWidget(parent)
{
    List_ = list;
    main_ = main;
    previous = parent;
    this->Init();
}

void MessageWindow::Init()
{
    next = new Message2Window(List_, this);
    List_->addWidget(next);
    QVBoxLayout *box = new QVBoxLayout(this);
    QLabel *message = new QLabel("Message 1", this);
    nextButton_ = new QPushButton("Next");
    previousButton_ = new QPushButton("Previous");
    QHBoxLayout *HLayout_ = new QHBoxLayout(this);
    HLayout_->addWidget(previousButton_);
    HLayout_->addWidget(nextButton_);
    message->setAlignment(Qt::AlignCenter);
    box->addWidget(message);
    box->addLayout(HLayout_);
    // setLayout(box);
    setWindowTitle("Message fenetre 1");

    connect(nextButton_, &QPushButton::clicked, this, &MessageWindow::nextWindow);
    connect(previousButton_, &QPushButton::clicked, this, &MessageWindow::previousWindow);

}

void MessageWindow::nextWindow()
{
    List_->setCurrentWidget(next);
}
void MessageWindow::previousWindow()
{
    List_->setCurrentWidget(previous);
    main_->PlayVideo();
}

MessageWindow::~MessageWindow()
{
}