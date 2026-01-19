#include "LoadingDialog.h"


LoadingDialog::LoadingDialog(const QString& message, QWidget* parent)
    : QWidget(parent)
{
    m_label = new QLabel(message, this);
    m_label->setAlignment(Qt::AlignCenter);

    m_progress = new QProgressBar(this);
    m_progress->setRange(0, 0);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_label);
    layout->addWidget(m_progress);

    setLayout(layout);
}

void LoadingDialog::setMessage(const QString& message)
{
    m_label->setText(message);
}