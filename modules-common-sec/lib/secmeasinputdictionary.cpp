#include "secmeasinputdictionary.h"

void SecMeasInputDictionary::addReferenceInput(const QString &inputFName, const QString &resource)
{
    Q_ASSERT(!m_resourceHash.contains(inputFName));
    m_resourceHash[inputFName] = resource;
}

void SecMeasInputDictionary::setAlias(const QString &inputFName, const QString &alias)
{
    Q_ASSERT(!m_aliasMap.contains(inputFName));
    m_aliasMap[inputFName] = alias;
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
    if(m_aliasMap.contains(inputFName))
        return m_aliasMap[inputFName];
    qWarning("Alias for input name %s not found", qPrintable(inputFName));
    return QString("P");
}

void SecMeasInputDictionary::setNotificationId(const QString &inputFName, int notificationId)
{
    Q_ASSERT(!m_notificationIdMap.contains(notificationId));
    m_notificationIdMap[notificationId] = inputFName;
}

QString SecMeasInputDictionary::getInputNameFromNotificationId(int notificationId) const
{
    Q_ASSERT(m_notificationIdMap.contains(notificationId));
    return m_notificationIdMap[notificationId];
}

QString SecMeasInputDictionary::getInputFNameFromAlias(const QString &alias) const
{
    Q_ASSERT(m_reverseAliasHash.contains(alias));
    return m_reverseAliasHash[alias];
}

QStringList SecMeasInputDictionary::getInputNameList() const
{
    return m_resourceHash.keys();
}

QStringList SecMeasInputDictionary::getInputAliasList() const
{
    return m_aliasMap.values();
}
