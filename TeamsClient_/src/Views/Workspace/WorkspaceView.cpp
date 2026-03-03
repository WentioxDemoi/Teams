#include "WorkspaceView.h"
// // ========= VisioConf POC =========
// #include "Visio/Visio.h"

WorkspaceView::WorkspaceView(QWidget *parent)
    : QWidget(parent)
{


    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);

    mainLayout_->addWidget(titleLabel_);
    
   
    


    // ========= VisioConf POC =========
    // visio_ = new Visio(this);
    // visioButton_ = new QPushButton("Start Visio"); // Avec le client d@gmail.com d
    // receiveButton_ = new QPushButton("Start Receive"); // Avec le client e@gmail.com e
    // mainLayout_->addWidget(visioButton_);
    // mainLayout_->addWidget(receiveButton_);
    // connect(visioButton_, &QPushButton::pressed, this, &WorkspaceView::startSender);
    // connect(receiveButton_, &QPushButton::pressed, this, &WorkspaceView::startReceiver);
    // connect(this, &WorkspaceView::OnP2PChange, visio_, &Visio::OnP2PChange);


    setLayout(mainLayout_);
    
}

// // ========= VisioConf POC =========
// void WorkspaceView::startReceiver()
// {
//      uuidd = "2211cb77-178b-443f-89dd-dcf803b75ae9";
//     //2211cb77-178b-443f-89dd-dcf803b75ae9
//     visio_->startReceiver();
// }
// // ========= VisioConf POC =========
// void WorkspaceView::startSender()
// {
//     uuidd = "8295ac0c-142d-43e1-9554-d4d271a7686a";
//     //"8295ac0c-142d-43e1-9554-d4d271a7686a"
//     emit initP2P();
//     visio_->startSender();
// }

