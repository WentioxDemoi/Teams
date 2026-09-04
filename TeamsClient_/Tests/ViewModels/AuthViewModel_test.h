#ifndef AUTH_VIEW_MODEL_TEST_H
#define AUTH_VIEW_MODEL_TEST_H

#include <QObject>

class AuthViewModelTest : public QObject {
  Q_OBJECT
 private slots:
  void startDelegatesTokenLogin();
  void loginRejectsMissingCredentials();
  void loginDelegatesValidCredentials();
  void registerRejectsMissingFields();
  void registerRejectsInvalidEmail();
  void registerDelegatesValidFields();
  void forwardsAuthSignals();
};

#endif