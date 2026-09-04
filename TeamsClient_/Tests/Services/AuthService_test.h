#ifndef AUTH_SERVICE_TEST_H
#define AUTH_SERVICE_TEST_H
#include <QObject>
class AuthServiceTest : public QObject {
  Q_OBJECT
 private slots:
  void loginAndRegisterSendPayloads();
  void validResponseSavesUserAndEmitsSuccess();
  void invalidResponseEmitsError();
  void disconnectsNetwork();
  void loginWithTokenUsesTokenFlow();
  void tokenErrorCleansLocalServices();
  void serverErrorsAreForwarded();
};
#endif