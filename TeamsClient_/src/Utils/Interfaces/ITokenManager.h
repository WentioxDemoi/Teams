#ifndef ITOKENMANAGER_H
#define ITOKENMANAGER_H

#include "../../includes.h"

class ITokenManager : public QObject {
    Q_OBJECT
public:
    explicit ITokenManager(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ITokenManager() = default;
    virtual bool writeToken(const QString&) = 0;
    virtual bool deleteToken() = 0;
    virtual bool readToken() = 0;
    QString token;
};

#endif