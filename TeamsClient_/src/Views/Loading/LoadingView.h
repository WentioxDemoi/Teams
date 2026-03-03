#ifndef LOADINGVIEW_H
#define LOADINGVIEW_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QString>

class LoadingView : public QWidget
{
    Q_OBJECT

public:
    explicit LoadingView(const QString& message,
                           QWidget* parent = nullptr);

    void setMessage(const QString& message);

private:
    QLabel* m_label;
    QProgressBar* m_progress;
};

#endif