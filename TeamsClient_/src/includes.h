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
#include <QPermission>
#include <QTimer>



// ======================================================
// Qt SQL
// ======================================================
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <qt6keychain/keychain.h>
#include <qt6keychain/qkeychain_export.h>
#include <QDir>
#include <QFile>
#include <QSqlDriver>




// ======================================================
// Standard Library
// ======================================================
#include <functional>
#include <typeinfo>
#include <unistd.h>


// ======================================================
// External Libraries (OpenSSL, etc.)
// ======================================================
#include <openssl/ssl.h>
#include <openssl/err.h>



#include <api/peer_connection_interface.h>
#include <api/create_peerconnection_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <rtc_base/ref_counted_object.h>

#include <QSslSocket>
#include <QByteArray>

#include <api/media_stream_interface.h>
#include <rtc_base/ref_counted_object.h>

#include <memory>
#include <string>

#define APP_NAME "Teams"

#endif