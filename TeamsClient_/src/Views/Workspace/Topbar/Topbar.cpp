#include "Topbar.h"
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qsizepolicy.h>

Topbar::Topbar(QWidget *parent)
    : QFrame(parent)
{
    setFixedHeight(48);
    setObjectName("Topbar");

    mainLayout_ = new QHBoxLayout(this);
    mainLayout_->setContentsMargins(8, 4, 8, 4);
    mainLayout_->setSpacing(8);

    searchInput_ = new QLineEdit(this);
    profilButton_ = new QPushButton("TMP", this);

    searchInput_->setPlaceholderText("Search...");

    // --- Layout structure ---
    mainLayout_->addStretch(5);

    mainLayout_->addWidget(searchInput_);

    mainLayout_->addStretch(3);

    mainLayout_->addWidget(profilButton_);


    setStyleSheet("QFrame#Topbar { border: 1px solid red; }");
}

void Topbar::initProfilButton()
{
    profilButton_->setIcon(QIcon(":/images/profil.png")); // A voir plus tard
    profilButton_->setIconSize(QSize(32, 32));
    profilButton_->setText("");
    //profilButton_->setFlat(true);
}

void Topbar::displayTBResults(const QVector<User>& users)
{

}