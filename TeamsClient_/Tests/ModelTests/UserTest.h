#ifndef USERTEST_H
#define USERTEST_H

#include <QtTest>
#include "User.h"

class UserTest : public QObject {
    Q_OBJECT
private slots:
    void testDefaultConstructor();
    void testParameterizedConstructor();
    void testSettersAndGetters();
    void testIsValidRequiresIdAndEmail();
    void testToJsonSerialization();
    void testFromJsonDeserialization();
    void testFromJsonWithMissingFields();
    void testFromJsonWithInvalidTypes();
};

#endif
