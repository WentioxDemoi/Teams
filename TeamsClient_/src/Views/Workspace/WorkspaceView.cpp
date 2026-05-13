#include "WorkspaceView.h"
#include "Workspace/Topbar/Topbar.h"

// ========= VisioConf POC =========
// #include "includes.h"

WorkspaceView::WorkspaceView(QWidget* parent) : QWidget(parent) {

  topbar_ = new Topbar(this);

  mainLayout_ = new QVBoxLayout(this);
  mainLayout_->setContentsMargins(0, 0, 0, 0);
  mainLayout_->setSpacing(0);

  mainLayout_->addWidget(topbar_);
  mainLayout_->addStretch();

  setLayout(mainLayout_);
}