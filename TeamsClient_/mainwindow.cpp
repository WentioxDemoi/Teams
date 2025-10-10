#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , VideoWidget_(new QVideoWidget(this))
    , MediaPlayer_(new QMediaPlayer(this))
    , PushButton_(new QPushButton("Lire la vidéo", this))
    , MessageButton_(new QPushButton("Messages", this))
    , Messages_(nullptr)
{
    ui->setupUi(this);
    Vues = new QStackedWidget(this);
    QWidget *central_ = new QWidget();
    QVBoxLayout *Vlayout_ = new QVBoxLayout(central_);
    Vlayout_->addWidget(VideoWidget_);
    QHBoxLayout *Hlayout_ = new QHBoxLayout();
    Hlayout_->addWidget(PushButton_);
    Hlayout_->addWidget(MessageButton_);
    Vlayout_->addLayout(Hlayout_);
    setCentralWidget(Vues);

    Vues->addWidget(central_);
    // Instantiate Messages_ after Vues and central_ exist, pass central_ as previous
    Messages_ = new MessageWindow(Vues, central_, this);
    Vues->addWidget(Messages_);

    connect(PushButton_, &QPushButton::clicked, this, &MainWindow::PlayVideo);
    connect(MessageButton_, &QPushButton::clicked, this, &MainWindow::OpenMessageWindow);

    MediaPlayer_->setVideoOutput(VideoWidget_);
    Vues->setCurrentWidget(central_);

}

void MainWindow::PlayVideo()
{
    MediaPlayer_->setSource(QUrl::fromLocalFile("/Users/remidesbordes/Desktop/video.mov"));
    MediaPlayer_->play();
}

void MainWindow::OpenMessageWindow()
{
    Vues->setCurrentWidget(Messages_);
}

MainWindow::~MainWindow()
{
    delete ui;
}