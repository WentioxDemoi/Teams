// #ifndef NETWORKSERVICE_H
// #define NETWORKSERVICE_H

// #include "../../includes.h"

// class NetworkService : public QObject {
//     public :
//         explicit NetworkService(QObject *parent = nullptr);
//         void post(const QUrl &url, const QByteArray &data, const QString &contentType = "application/json");
//         void get(const QUrl &url);

//     signals : 
//         void reponseReceived(const QByteArray &data);
//         void errorOccurred(const QString &error);

//     private slots :
//         void onFinished(QNetworkReply *reply);
    
//     private :
//         QNetworkAccessManager *manager;

        
// };

// #endif