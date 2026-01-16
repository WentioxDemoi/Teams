#include "TokenManager.h"

TokenManager::TokenManager(QObject *parent) : QObject(parent), m_readCredentialJob(APP_NAME), m_writeCredentialJob(APP_NAME), m_deleteCredentialJob(APP_NAME)
{
    m_readCredentialJob.setAutoDelete(false);
    m_writeCredentialJob.setAutoDelete(false);
    m_deleteCredentialJob.setAutoDelete(false);
}

void TokenManager::readKey(const QString &key)
{
    m_readCredentialJob.setKey(key);

    QObject::connect(&m_readCredentialJob, &QKeychain::ReadPasswordJob::finished, [this, key]()
                     {
        if (m_readCredentialJob.error()) {
            emit error(
                    tr("Read key failed: %1").arg(qPrintable(m_readCredentialJob.errorString())));
            return;
        }
        emit keyRestored(key, m_readCredentialJob.textData()); });

    m_readCredentialJob.start();
}

void TokenManager::writeKey(const QString &key, const QString &value)
{
    m_writeCredentialJob.setKey(key);

    QObject::connect(&m_writeCredentialJob, &QKeychain::WritePasswordJob::finished, [this, key]()
                     {
        if (m_writeCredentialJob.error()) {
            emit error(
                    tr("Write key failed: %1").arg(qPrintable(m_writeCredentialJob.errorString())));
            return;
        }

        emit keyStored(key); });

    m_writeCredentialJob.setTextData(value);
    m_writeCredentialJob.start();
}

void TokenManager::deleteKey(const QString &key)
{
    m_deleteCredentialJob.setKey(key);

    QObject::connect(&m_deleteCredentialJob, &QKeychain::DeletePasswordJob::finished, [this, key]()
                     {
        if (m_deleteCredentialJob.error()) {
            emit error(tr("Delete key failed: %1")
                               .arg(qPrintable(m_deleteCredentialJob.errorString())));
            return;
        }
        emit keyDeleted(key); });

    m_deleteCredentialJob.start();
}