#ifndef FRAME_CONVERTER_TEST_H
#define FRAME_CONVERTER_TEST_H

#include <QObject>

class FrameConverterTest : public QObject {
  Q_OBJECT

 private slots:
  void invalidInputsReturnEmptyResults();
  void nv12ToI420ConvertsPlanes();
  void i420ToVideoFrameConvertsPlanes();
};

#endif