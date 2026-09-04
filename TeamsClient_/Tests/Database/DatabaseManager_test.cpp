#include "DatabaseManager_test.h"

#include "Database/DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QtTest/QtTest>

void DatabaseManagerTest::instanceIsSingleton() {
  QCOMPARE(&DatabaseManager::instance(), &DatabaseManager::instance());
}

void DatabaseManagerTest::databaseConnectionIsOpen() {
  const QSqlDatabase database = DatabaseManager::instance().database();

  QVERIFY(database.isValid());
  QVERIFY(database.isOpen());
  QCOMPARE(database.connectionName(), QStringLiteral("AppConnection"));
  QCOMPARE(database.driverName(), QStringLiteral("QSQLITE"));
}

void DatabaseManagerTest::expectedTablesExist() {
  const QSqlDatabase database = DatabaseManager::instance().database();
  const QStringList tables = database.tables();

  QVERIFY(tables.contains(QStringLiteral("users")));
  QVERIFY(tables.contains(QStringLiteral("messages")));
}

void DatabaseManagerTest::usersTableHasExpectedSchema() {
  const QSqlDatabase database = DatabaseManager::instance().database();
  const QSqlRecord record = database.record(QStringLiteral("users"));
  const QStringList expectedColumns{
      "email", "first_name", "last_name", "status", "uuid", "is_me", "token", "avatar", "last_read_at"};

  for (const QString& column : expectedColumns) {
    bool found = false;
    for (int index = 0; index < record.count(); ++index) {
      found |= record.fieldName(index) == column;
    }
    QVERIFY2(found, qPrintable("Missing users column: " + column));
  }
}

void DatabaseManagerTest::messagesTableHasExpectedSchema() {
  const QSqlDatabase database = DatabaseManager::instance().database();
  const QSqlRecord record = database.record(QStringLiteral("messages"));
  const QStringList expectedColumns{
      "uuid", "sender_uuid", "receiver_uuid", "chat_type", "content", "timestamp"};

  for (const QString& column : expectedColumns) {
    bool found = false;
    for (int index = 0; index < record.count(); ++index) {
      found |= record.fieldName(index) == column;
    }
    QVERIFY2(found, qPrintable("Missing messages column: " + column));
  }
}

void DatabaseManagerTest::repeatedAccessUsesSameConnection() {
  const QSqlDatabase first = DatabaseManager::instance().database();
  const QSqlDatabase second = DatabaseManager::instance().database();

  QCOMPARE(first.connectionName(), second.connectionName());
  QCOMPARE(QSqlDatabase::connectionNames().count(QStringLiteral("AppConnection")), 1);

  QSqlQuery query(first);
  QVERIFY(query.exec(QStringLiteral("SELECT 1")));
  QVERIFY(query.next());
  QCOMPARE(query.value(0).toInt(), 1);
}

void DatabaseManagerTest::databaseUsesApplicationDataPath() {
  const QSqlDatabase database = DatabaseManager::instance().database();
  const QString expectedDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

  QVERIFY(!database.databaseName().isEmpty());
  QVERIFY(database.databaseName().startsWith(expectedDirectory));
  QVERIFY(database.databaseName().endsWith(QStringLiteral("app_database.db")));
}

void DatabaseManagerTest::usersTableHasExpectedConstraints() {
  QSqlQuery query(DatabaseManager::instance().database());
  QVERIFY(query.exec(QStringLiteral("PRAGMA table_info(users)")));

  QHash<QString, QVariantMap> columns;
  while (query.next()) {
    QVariantMap column;
    column.insert(QStringLiteral("notnull"), query.value(3));
    column.insert(QStringLiteral("pk"), query.value(5));
    column.insert(QStringLiteral("default"), query.value(4));
    columns.insert(query.value(1).toString(), column);
  }

  QVERIFY(columns.contains(QStringLiteral("email")));
  QVERIFY(columns.contains(QStringLiteral("first_name")));
  QVERIFY(columns.contains(QStringLiteral("last_name")));
  QVERIFY(columns.contains(QStringLiteral("uuid")));
  QCOMPARE(columns.value("email").value("notnull").toInt(), 1);
  QCOMPARE(columns.value("first_name").value("notnull").toInt(), 1);
  QCOMPARE(columns.value("last_name").value("notnull").toInt(), 1);
  QCOMPARE(columns.value("uuid").value("pk").toInt(), 1);
  QCOMPARE(columns.value("is_me").value("default").toString(), QStringLiteral("0"));
}

void DatabaseManagerTest::messagesTableHasExpectedConstraints() {
  QSqlQuery query(DatabaseManager::instance().database());
  QVERIFY(query.exec(QStringLiteral("PRAGMA table_info(messages)")));

  QHash<QString, QVariantMap> columns;
  while (query.next()) {
    QVariantMap column;
    column.insert(QStringLiteral("notnull"), query.value(3));
    column.insert(QStringLiteral("pk"), query.value(5));
    columns.insert(query.value(1).toString(), column);
  }

  QVERIFY(columns.contains(QStringLiteral("uuid")));
  QVERIFY(columns.contains(QStringLiteral("sender_uuid")));
  QVERIFY(columns.contains(QStringLiteral("receiver_uuid")));
  QCOMPARE(columns.value("uuid").value("pk").toInt(), 1);
  QCOMPARE(columns.value("sender_uuid").value("notnull").toInt(), 1);
  QCOMPARE(columns.value("receiver_uuid").value("notnull").toInt(), 1);
}

void DatabaseManagerTest::databaseSupportsTransactions() {
  QSqlDatabase database = DatabaseManager::instance().database();
  QVERIFY(database.transaction());

  QSqlQuery query(database);
  QVERIFY(query.exec(QStringLiteral("CREATE TEMP TABLE database_manager_test (value INTEGER)")));
  QVERIFY(database.rollback());

  QVERIFY(query.exec(QStringLiteral(
      "SELECT COUNT(*) FROM sqlite_temp_master WHERE type = 'table' AND name = 'database_manager_test'")));
  QVERIFY(query.next());
  QCOMPARE(query.value(0).toInt(), 0);
}