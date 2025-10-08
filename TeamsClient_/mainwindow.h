#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QMediaPlayer>
#include <QtMultimediaWidgets/QVideoWidget>
// #include <QTMu QVideoWidget>
#include <QPushButton>
#include <QVBoxLayout>


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

private:
    Ui::MainWindow *ui;
    QMediaPlayer *MediaPlayer_;
    QVideoWidget *VideoWidget_;
    QPushButton *PushButton_;
};
#endif // MAINWINDOW_H
