#ifndef WORKSPACEVIEW_H
#define WORKSPACEVIEW_H

#include <QWidget>

class QLabel;
class QVBoxLayout;

class WorkspaceView : public QWidget
{
    Q_OBJECT

public:
    explicit WorkspaceView(QWidget *parent = nullptr);

private:
    QLabel *titleLabel_;
    QWidget *contentWidget_;
    QVBoxLayout *mainLayout_;
};

#endif // WORKSPACEVIEW_H