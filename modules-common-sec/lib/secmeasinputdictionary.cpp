#include "secmeasinputdictionary.h"

void SecMeasInputDictionary::addReferenceInput(const QString &inputName, const QString &resource)
{
    Q_ASSERT(!m_resourceHash.contains(inputName));
    m_resourceHash[inputName] = resource;
}

void SecMeasInputDictionary::setAlias(const QString &inputName, const QString &alias)
{
    Q_ASSERT(!m_aliasHash.contains(inputName));
    m_aliasHash[inputName] = alias;
}

void SecMeasInputDictionary::setDisplayedString(const QString &inputName, const QString &displayName)
{
    Q_ASSERT(!m_displayHash.contains(displayName));
    m_displayHash[displayName] = inputName;
}

QString SecMeasInputDictionary::getResource(const QString &inputName) const
{
    Q_ASSERT(m_resourceHash.contains(inputName));
    return m_resourceHash[inputName];
}

QString SecMeasInputDictionary::getAlias(const QString &inputName) const
{
    if(m_aliasHash.contains(inputName))
        return m_aliasHash[inputName];
    qWarning("Alias for input name %s not found", qPrintable(inputName));
    return QString("P");
}

QString SecMeasInputDictionary::getInputNameFromDisplayedName(const QString &displayName) const
{
    Q_ASSERT(m_displayHash.contains(displayName));
    return m_displayHash[displayName];
}

void SecMeasInputDictionary::setNotificationId(const QString &inputName, int notificationId)
{
    Q_ASSERT(!m_notificationIdHash.contains(notificationId));
    m_notificationIdHash[notificationId] = inputName;
}

QString SecMeasInputDictionary::getInputNameFromNotificationId(int notificationId)
{
    Q_ASSERT(m_notificationIdHash.contains(notificationId));
    return m_notificationIdHash[notificationId];
}

QStringList SecMeasInputDictionary::getInputNameList()
{
    return m_resourceHash.keys();
}
