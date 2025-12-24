// #include "NetworkService.h"

// NetworkService::NetworkService(QObject *parent) : QObject(parent) {
//     manager = new QNetworkAccessManager(this);
//     connect(manager, &QNetworkAccessManager::finished, this, &NetworkService::onFinished);
// }

// void NetworkService::get(const QUrl &url)
// {
//     QNetworkRequest request(url);

//     QSslConfiguration sslConfig = request.sslConfiguration();
//     sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone); //Pour certificats autosigné
//     request.setSslConfiguration(sslConfig);

//     manager->get(request);
// }

// void NetworkService::post(const QUrl &url, const QByteArray &data, const QString &contentType = "application/json")
// {
//     QNetworkRequest request(url);
//     request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);

//     QSslConfiguration sslConfig = request.sslConfiguration();
//     sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone); //Pour certificats autosigné

//     request.setSslConfiguration(sslConfig);

//     manager->post(request, data);
// }

// void NetworkService::onFinished(QNetworkReply *reply)
// {
//     if (reply->error() != QNetworkReply::NoError) {
//         emit errorOccurred(reply->errorString());
//     } else
//         emit reponseReceived(reply->readAll());

//     reply->deleteLater();
// }