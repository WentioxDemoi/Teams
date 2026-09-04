#ifndef CONTACT_SERVICE_TEST_H
#define CONTACT_SERVICE_TEST_H

#include <QObject>

class ContactServiceTest : public QObject {
  Q_OBJECT

 private slots:
  void searchUsersSendsRequestAndIgnoresBlankQuery();
  void resolveUserSendsRequest();
  void saveContactPersistsAndSendsRequest();
  void saveContactFailureEmitsError();
  void updateLastReadAtPersistsBeforeSending();
  void updateLastReadAtFailureDoesNotSend();
  void contactsLoadedPersistsAndEmitsUsers();
  void searchResponseEmitsUsers();
  void resolveResponseSavesAndEmitsUser();
  void statusUpdateEmitsStatus();
  void malformedResponsesEmitErrors();
  void deleteAllAndDisconnectDelegate();
};

#endif