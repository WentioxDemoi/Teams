#include "FrameConverter_test.h"

#include "Utils/FrameConverter.h"

#include <QTest>
#include <QVideoFrameFormat>

namespace {
constexpr int kWidth = 4;
constexpr int kHeight = 2;
}

void FrameConverterTest::invalidInputsReturnEmptyResults() {
  QVERIFY(FrameConverter::NV12ToI420(QVideoFrame()) == nullptr);
  QVERIFY(!FrameConverter::I420ToVideoFrame(nullptr).isValid());
}

void FrameConverterTest::nv12ToI420ConvertsPlanes() {
  QVideoFrame frame(QVideoFrameFormat(QSize(kWidth, kHeight), QVideoFrameFormat::Format_NV12));
  QVERIFY(frame.map(QVideoFrame::WriteOnly));

  uchar* yPlane = frame.bits(0);
  uchar* uvPlane = frame.bits(1);
  QVERIFY(yPlane != nullptr);
  QVERIFY(uvPlane != nullptr);

  const uchar expectedY[] = {16, 32, 48, 64, 80, 96, 112, 128};
  const uchar expectedU[] = {90, 100};
  const uchar expectedV[] = {140, 150};

  for (int column = 0; column < kWidth; ++column) {
    yPlane[column] = expectedY[column];
    yPlane[frame.bytesPerLine(0) + column] = expectedY[kWidth + column];
  }
  for (int pair = 0; pair < kWidth / 2; ++pair) {
    uvPlane[2 * pair] = expectedU[pair];
    uvPlane[2 * pair + 1] = expectedV[pair];
  }
  frame.unmap();

  const auto converted = FrameConverter::NV12ToI420(frame);
  QVERIFY(converted != nullptr);
  QCOMPARE(converted->width(), kWidth);
  QCOMPARE(converted->height(), kHeight);

  for (int row = 0; row < kHeight; ++row) {
    for (int column = 0; column < kWidth; ++column) {
      QCOMPARE(converted->DataY()[row * converted->StrideY() + column],
               expectedY[row * kWidth + column]);
    }
  }
  for (int column = 0; column < kWidth / 2; ++column) {
    QCOMPARE(converted->DataU()[column], expectedU[column]);
    QCOMPARE(converted->DataV()[column], expectedV[column]);
  }
}

void FrameConverterTest::i420ToVideoFrameConvertsPlanes() {
  const auto buffer = webrtc::I420Buffer::Create(kWidth, kHeight);
  const uchar expectedY[] = {16, 32, 48, 64, 80, 96, 112, 128};
  const uchar expectedU[] = {90, 100};
  const uchar expectedV[] = {140, 150};

  for (int row = 0; row < kHeight; ++row) {
    for (int column = 0; column < kWidth; ++column) {
      buffer->MutableDataY()[row * buffer->StrideY() + column] =
          expectedY[row * kWidth + column];
    }
  }
  for (int column = 0; column < kWidth / 2; ++column) {
    buffer->MutableDataU()[column] = expectedU[column];
    buffer->MutableDataV()[column] = expectedV[column];
  }

  QVideoFrame frame = FrameConverter::I420ToVideoFrame(buffer);
  QVERIFY(frame.isValid());
  QCOMPARE(frame.size(), QSize(kWidth, kHeight));
  QCOMPARE(frame.pixelFormat(), QVideoFrameFormat::Format_NV12);
  QVERIFY(frame.map(QVideoFrame::ReadOnly));

  const uchar* yPlane = frame.bits(0);
  const uchar* uvPlane = frame.bits(1);
  QVERIFY(yPlane != nullptr);
  QVERIFY(uvPlane != nullptr);

  for (int row = 0; row < kHeight; ++row) {
    for (int column = 0; column < kWidth; ++column) {
      QCOMPARE(yPlane[row * frame.bytesPerLine(0) + column],
               expectedY[row * kWidth + column]);
    }
  }
  for (int pair = 0; pair < kWidth / 2; ++pair) {
    QCOMPARE(uvPlane[2 * pair], expectedU[pair]);
    QCOMPARE(uvPlane[2 * pair + 1], expectedV[pair]);
  }
  frame.unmap();
}