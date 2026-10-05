#include "secmeasinputdictionary.h"

void SecMeasInputDictionary::addReferenceInput(const QString &inputFName, const QString &resource)
{
    Q_ASSERT(!m_resourceHash.contains(inputFName));
    m_resourceHash[inputFName] = resource;
}

void SecMeasInputDictionary::setAlias(const QString &inputFName, const QString &alias)
{
    Q_ASSERT(!m_aliasHash.contains(inputFName));
    m_aliasHash[inputFName] = alias;
    Q_ASSERT(!m_reverseAliasHash.contains(alias));
    m_reverseAliasHash[alias] = inputFName;
}

QString SecMeasInputDictionary::getResource(const QString &inputFName) const
{
    Q_ASSERT(m_resourceHash.contains(inputFName));
    return m_resourceHash[inputFName];
}

QString SecMeasInputDictionary::getAlias(const QString &inputFName) const
{
    if(m_aliasHash.contains(inputFName))
        return m_aliasHash[inputFName];
    qWarning("Alias for input name %s not found", qPrintable(inputFName));
    return QString("P");
}

void SecMeasInputDictionary::setNotificationId(const QString &inputFName, int notificationId)
{
    Q_ASSERT(!m_notificationIdHash.contains(notificationId));
    m_notificationIdHash[notificationId] = inputFName;
}

QString SecMeasInputDictionary::getInputNameFromNotificationId(int notificationId)
{
    Q_ASSERT(m_notificationIdHash.contains(notificationId));
    return m_notificationIdHash[notificationId];
}

QString SecMeasInputDictionary::getInputFNameFromAlias(const QString &alias) const
{
    Q_ASSERT(m_reverseAliasHash.contains(alias));
    return m_reverseAliasHash[alias];
}

QStringList SecMeasInputDictionary::getInputNameList()
{
    return m_resourceHash.keys();
}
