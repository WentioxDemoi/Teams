#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QMediaPlayer>
#include <QtMultimediaWidgets/QVideoWidget>
// #include <QTMu QVideoWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>
#include "messagewindow.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void PlayVideo();
    void OpenMessageWindow();

private:
    Ui::MainWindow *ui;
    QStackedWidget *Vues;
    QMediaPlayer *MediaPlayer_;
    QVideoWidget *VideoWidget_;
    QPushButton *PushButton_;
    QPushButton *MessageButton_;
    MessageWindow *Messages_;
};
#endif // MAINWINDOW_H