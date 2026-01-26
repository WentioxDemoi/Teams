#include "TokenManager.h"

TokenManager &TokenManager::instance()
{
    static TokenManager instance;
    return instance;
}

TokenManager::TokenManager(QObject *parent) 
    : QObject(parent), 
      m_readCredentialJob(APP_NAME), 
      m_writeCredentialJob(APP_NAME), 
      m_deleteCredentialJob(APP_NAME)
{
    m_readCredentialJob.setAutoDelete(false);
    m_writeCredentialJob.setAutoDelete(false);
    m_deleteCredentialJob.setAutoDelete(false);
    readToken();

}

bool TokenManager::readToken()
{
    m_readCredentialJob.setKey("Token");

    QEventLoop loop;
    QObject::connect(&m_readCredentialJob, &QKeychain::ReadPasswordJob::finished, &loop, &QEventLoop::quit);
    m_readCredentialJob.start();
    loop.exec();

    if (m_readCredentialJob.error()) {
        qWarning() << "Read key failed:" << m_readCredentialJob.errorString();
        token.clear();
        return false;
    }

    token = m_readCredentialJob.textData();
    return true;
}

bool TokenManager::writeToken(const QString &value)
{
    m_writeCredentialJob.setKey("Token");
    m_writeCredentialJob.setTextData(value);

    QEventLoop loop;
    QObject::connect(&m_writeCredentialJob, &QKeychain::WritePasswordJob::finished, &loop, &QEventLoop::quit);
    m_writeCredentialJob.start();
    loop.exec();

    if (m_writeCredentialJob.error()) {
        qWarning() << "Write key failed:" << m_writeCredentialJob.errorString();
        return false;
    }

    token = value;
    return true;
}

bool TokenManager::deleteToken()
{
    m_deleteCredentialJob.setKey("Token");

    QEventLoop loop;
    QObject::connect(&m_deleteCredentialJob, &QKeychain::DeletePasswordJob::finished, &loop, &QEventLoop::quit);
    m_deleteCredentialJob.start();
    loop.exec();

    if (m_deleteCredentialJob.error()) {
        qWarning() << "Delete key failed:" << m_deleteCredentialJob.errorString();
        return false;
    }

    token.clear();
    return true;
}