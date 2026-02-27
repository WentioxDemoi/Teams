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
    receiveButton_ = new QPushButton("Start Receive");
    mainLayout_->addWidget(receiveButton_);
    connect(visioButton_, &QPushButton::pressed, this, &WorkspaceView::startSender);
    connect(receiveButton_, &QPushButton::pressed, this, &WorkspaceView::startReceiver);
    visio_ = new Visio(this);
    setLayout(mainLayout_);
    connect(this, &WorkspaceView::OnP2PChange, visio_, &Visio::OnP2PChange);
}


void WorkspaceView::startReceiver()
{
     uuidd = "2211cb77-178b-443f-89dd-dcf803b75ae9";
    //2211cb77-178b-443f-89dd-dcf803b75ae9
    visio_->startReceiver();
}

void WorkspaceView::startSender()
{
    uuidd = "8295ac0c-142d-43e1-9554-d4d271a7686a";
    //"8295ac0c-142d-43e1-9554-d4d271a7686a"
    emit initP2P();
    visio_->startSender();
}

