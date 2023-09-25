#include "secmeasinputdictionary.h"

void SecMeasInputDictionary::addReferenceInput(QString inputName, QString resource)
{
    Q_ASSERT(!m_resourceHash.contains(inputName));
    m_resourceHash[inputName] = resource;
}

void SecMeasInputDictionary::setAlias(QString inputName, QString alias)
{
    Q_ASSERT(!m_aliasHash.contains(inputName));
    m_aliasHash[inputName] = alias;
}

void SecMeasInputDictionary::setDisplayedString(QString inputName, QString displayName)
{
    Q_ASSERT(!m_displayHash.contains(displayName));
    m_displayHash[displayName] = inputName;
}

QString SecMeasInputDictionary::getResource(QString inputName) const
{
    Q_ASSERT(m_resourceHash.contains(inputName));
    return m_resourceHash[inputName];
}

QString SecMeasInputDictionary::getAlias(QString inputName) const
{
    if(m_aliasHash.contains(inputName))
        return m_aliasHash[inputName];
    qWarning("Alias for input name %s not found", qPrintable(inputName));
    return QString("P");
}

QString SecMeasInputDictionary::getInputNameFromDisplayedName(QString displayName) const
{
    Q_ASSERT(m_displayHash.contains(displayName));
    return m_displayHash[displayName];
}

void SecMeasInputDictionary::setNotificationId(QString inputName, int notificationId)
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
