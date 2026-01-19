#ifndef INCLUDES_H
#define INCLUDES_H

// ======================================================
// Qt Core & GUI
// ======================================================
#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QObject>
#include <QRegularExpression>
#include <QMap>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QLabel>
#include <QProgressBar>
#include <QDialog>
#include <QStackedWidget>

// ======================================================
// Qt SQL
// ======================================================
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <qt6keychain/keychain.h>
#include <qt6keychain/qkeychain_export.h>



// ======================================================
// Standard Library
// ======================================================
#include <functional>
#include <typeinfo>

// ======================================================
// External Libraries (OpenSSL, etc.)
// ======================================================
#include <openssl/ssl.h>
#include <openssl/err.h>

#define APP_NAME "Teams"

#endif