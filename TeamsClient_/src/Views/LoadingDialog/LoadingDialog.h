#ifndef LOADINGDIALOG_H
#define LOADINGDIALOG_H

#include "../../includes.h"

class LoadingDialog : public QWidget
{
    Q_OBJECT

public:
    explicit LoadingDialog(const QString& message,
                           QWidget* parent = nullptr);

    void setMessage(const QString& message);

private:
    QLabel* m_label;
    QProgressBar* m_progress;
};

#endif