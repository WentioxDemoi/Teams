#ifndef WORKSPACEVIEW_H
#define WORKSPACEVIEW_H

#include "../../includes.h"
#include "../Visio/Visio.h"

class QLabel;
class QVBoxLayout;

class WorkspaceView : public QWidget
{
    Q_OBJECT

public:
    explicit WorkspaceView(QWidget *parent = nullptr);
    void startSender();
    void startReceiver();

signals:
    void startVisio();
    void OnP2PChange(bool inProgress);
    void initP2P();

public slots:
    

private:
    QLabel *titleLabel_;
    QWidget *contentWidget_;
    QVBoxLayout *mainLayout_;
    QPushButton *visioButton_;
     QPushButton *receiveButton_;
    Visio *visio_;
};

#endif