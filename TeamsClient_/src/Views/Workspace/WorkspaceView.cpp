#include "WorkspaceView.h"
#include "Visio/Visio.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>

WorkspaceView::WorkspaceView(QWidget *parent)
    : QWidget(parent)
{
    // Titre
    titleLabel_ = new QLabel("Workspace", this);
    titleLabel_->setAlignment(Qt::AlignCenter);
    titleLabel_->setStyleSheet(
        "font-size: 22px;"
        "font-weight: bold;"
        "padding: 16px;");

    // Zone centrale (contenu futur)
    contentWidget_ = new QFrame(this);
    contentWidget_->setStyleSheet(
        "background-color: #f5f5f5;"
        "border: 1px solid #ddd;");

    // Layout principal
    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);

    mainLayout_->addWidget(titleLabel_);
    mainLayout_->addWidget(contentWidget_, 1);
    visioButton_ = new QPushButton("Start Visio");
    mainLayout_->addWidget(visioButton_);
    connect(visioButton_, &QPushButton::pressed, this, &WorkspaceView::startSender);
    visio_ = new Visio(this);
    setLayout(mainLayout_);
    connect(this, &WorkspaceView::OnP2PChange, visio_, &Visio::OnP2PChange);
}


void WorkspaceView::startReceiver()
{
    visio_->startReceiver();
}

void WorkspaceView::startSender()
{
    emit initP2P();
    visio_->startSender();
}

