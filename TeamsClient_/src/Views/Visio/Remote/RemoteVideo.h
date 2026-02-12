#ifndef REMOTEVIDEO_H
#define REMOTEVIDEO_H

#include "../../../includes.h"

class RemoteVideo : public QWidget {
  Q_OBJECT

public:
  explicit RemoteVideo(QWidget *parent = nullptr) : QWidget(parent) {
        qDebug()<< "ok"; 
  };

private:


protected:
    

};

#endif