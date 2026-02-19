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
    void startVisio();

private:
    QLabel *titleLabel_;
    QWidget *contentWidget_;
    QVBoxLayout *mainLayout_;
    QPushButton *visioButton_;
    Visio *visio_;
};

#endif