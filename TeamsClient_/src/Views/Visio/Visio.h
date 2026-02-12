#ifndef VISIO_H
#define VISIO_H

#include "../../includes.h"
#include "Local/LocalVideo.h"
#include "Remote/RemoteVideo.h"

class Visio : public QMainWindow
{
    Q_OBJECT

public:
    explicit Visio(QWidget* parent = nullptr);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    void start();
    void updateLocalVideoPosition();

private:
    QWidget* mainWidget = nullptr;
    QVBoxLayout* layout = nullptr;

    RemoteVideo* remoteVideo = nullptr;
    LocalVideo* localVideo = nullptr;
};

#endif