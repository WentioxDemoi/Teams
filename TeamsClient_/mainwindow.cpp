#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , VideoWidget_(new QVideoWidget(this))
    , MediaPlayer_(new QMediaPlayer(this))
    , PushButton_(new QPushButton("Lire la vidéo", this))
{
    ui->setupUi(this);
    QWidget *central_ = new QWidget();
    QVBoxLayout *layout_ = new QVBoxLayout(central_);
    layout_->addWidget(VideoWidget_);
    layout_->addWidget(PushButton_);
    setCentralWidget(central_);

    connect(PushButton_, &QPushButton::clicked, this, &MainWindow::PlayVideo);

    MediaPlayer_->setVideoOutput(VideoWidget_);

}

void MainWindow::PlayVideo()
{
    MediaPlayer_->setSource(QUrl::fromLocalFile("/Users/remidesbordes/Desktop/video.mov"));
    MediaPlayer_->play();
}

MainWindow::~MainWindow()
{
    delete ui;
}
