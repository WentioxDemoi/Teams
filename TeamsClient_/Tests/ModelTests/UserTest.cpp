#include "UserTest.h"

void UserTest::testDefaultConstructor() {
    User user;
    QVERIFY(user.email().isEmpty());
    QVERIFY(user.username().isEmpty());
    QVERIFY(user.status().isEmpty());
    QCOMPARE(user.isMe(), false);
    QVERIFY(!user.isValid());
}

void UserTest::testParameterizedConstructor() {
    User user("test@mail.com", "john", "active", false, "token", "uuid");
    QCOMPARE(user.email(), QString("test@mail.com"));
    QCOMPARE(user.username(), QString("john"));
    QCOMPARE(user.status(), QString("active"));
    QCOMPARE(user.isMe(), false);
    QVERIFY(user.isValid());
}

void UserTest::testSettersAndGetters() {
    User user;
    user.setEmail("user@mail.com");
    user.setUsername("alice");
    user.setStatus("online");
    user.setIsMe(true);
    user.setToken("token");
    user.setUuid("uuid");

    QCOMPARE(user.email(), QString("user@mail.com"));
    QCOMPARE(user.username(), QString("alice"));
    QCOMPARE(user.status(), QString("online"));
    QCOMPARE(user.isMe(), true);
}

void UserTest::testToJsonSerialization() {
    User user("json@mail.com", "jsonuser", "busy", true);
    QJsonObject json = user.toJson();
    QCOMPARE(json["email"].toString(), QString("json@mail.com"));
    QCOMPARE(json["username"].toString(), QString("jsonuser"));
    QCOMPARE(json["status"].toString(), QString("busy"));
    QCOMPARE(json["isMe"].toBool(), true);
}

void UserTest::testFromJsonDeserialization() {
    QJsonObject json;
    json["email"] = "fromjson@mail.com";
    json["username"] = "bob";
    json["status"] = "offline";
    json["isMe"] = false;

    User user = User::fromJson(json);
    QCOMPARE(user.email(), QString("fromjson@mail.com"));
    QCOMPARE(user.username(), QString("bob"));
    QCOMPARE(user.status(), QString("offline"));
    QCOMPARE(user.isMe(), false);
}

void UserTest::testFromJsonWithMissingFields() {
    QJsonObject json;
    json["email"] = "partial@mail.com";

    User user = User::fromJson(json);
    QCOMPARE(user.email(), QString("partial@mail.com"));
    QCOMPARE(user.isMe(), false); // valeur par défaut
    QVERIFY(!user.isValid());
}

void UserTest::testFromJsonWithInvalidTypes() {
    QJsonObject json;
    json["email"] = 123;
    json["username"] = true;
    json["isMe"] = "yes";

    User user = User::fromJson(json);
    QVERIFY(user.email().isEmpty());
    QVERIFY(user.username().isEmpty());
    QCOMPARE(user.isMe(), false);
    QVERIFY(!user.isValid());
}