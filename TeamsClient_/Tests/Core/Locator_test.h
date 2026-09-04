#ifndef LOCATOR_TEST_H
#define LOCATOR_TEST_H

#include <QObject>

class LocatorTest : public QObject {
  Q_OBJECT

 private slots:
  void serviceLocatorRegistersReplacesAndClears();
  void stateLocatorRegistersReplacesAndClears();
  void modelLocatorRegistersReplacesAndClears();
  void viewModelsLocatorRegistersReplacesAndClears();
  void locatorsAreSingletonsAndKeepNamespacesSeparate();
};

#endif