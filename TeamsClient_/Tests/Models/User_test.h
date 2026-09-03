#ifndef USER_TEST_H
#define USER_TEST_H

#include <QObject>

class UserTest : public QObject {
  Q_OBJECT

 private slots:
  void defaultUserIsInvalid();
  void derivedPropertiesAreComputed();
  void validityRequiresEmailNameAndUuid();
  void jsonRoundTripPreservesSerializedFields();
  void fromJsonIgnoresWrongTypes();
  void avatarColorIsDeterministic();
  void setLastReadAtStoresUtc();
  void clearTokenRemovesToken();
};

#endif