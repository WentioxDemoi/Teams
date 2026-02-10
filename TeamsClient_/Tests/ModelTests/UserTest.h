#ifndef USERTEST_H
#define USERTEST_H

#include "User.h"
#include <QtTest>

class UserTest : public QObject {
    Q_OBJECT
private slots:
    void testDefaultConstructor();
    void testParameterizedConstructor();
    void testSettersAndGetters();
    void testToJsonSerialization();
    void testFromJsonDeserialization();
    void testFromJsonWithMissingFields();
    void testFromJsonWithInvalidTypes();
};

#endif
