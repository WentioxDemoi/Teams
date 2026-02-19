#include "Visio.h"

Visio::Visio(QWidget* parent)
    : QMainWindow(parent)
{
    resize(960, 540);
    setWindowTitle("Visio");

    start();
    show();
}

void Visio::start()
{
    // Widget central
    mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);

    layout = new QVBoxLayout(mainWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    // Vidéo distante (plein écran)
    remoteVideo = new RemoteVideo(mainWidget);
    layout->addWidget(remoteVideo);

    // Vidéo locale (overlay)
    localVideo = new LocalVideo(mainWidget);
    // localVideo->raise();

    updateLocalVideoPosition();
}

void Visio::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    updateLocalVideoPosition();
}

void Visio::updateLocalVideoPosition()
{
    if (!localVideo)
        return;

    int margin = 10;
    localVideo->move(
        width() - localVideo->width() - margin,
        height() - localVideo->height() - margin
    );
}

void Visio::closeEvent(QCloseEvent* event)
{
    // Plus tard : stop WebRTC, caméra, audio, etc.
    // event->accept();
}