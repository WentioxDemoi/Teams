#ifndef WORKSPACEVIEW_H
#define WORKSPACEVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>

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

    // // ========= VisioConf POC =========
    // Visio *visio_;
};

#endif