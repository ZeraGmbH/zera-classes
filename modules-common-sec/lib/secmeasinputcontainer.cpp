#include "secmeasinputcontainer.h"

SecMeasInputContainer::SecMeasInputContainer()
{
}

void SecMeasInputContainer::addReferenceInput(QString inputName, QString resource)
{
    Q_ASSERT(!m_resourceHash.contains(inputName));
    m_resourceHash[inputName] = resource;
}

void SecMeasInputContainer::setAlias(QString inputName, QString alias)
{
    Q_ASSERT(!m_aliasHash.contains(inputName));
    m_aliasHash[inputName] = alias;
}

void SecMeasInputContainer::setDisplayedString(QString inputName, QString displayName)
{
    Q_ASSERT(!m_displayHash.contains(displayName));
    m_displayHash[displayName] = inputName;
}

QString SecMeasInputContainer::getResource(QString inputName) const
{
    Q_ASSERT(m_resourceHash.contains(inputName));
    return m_resourceHash[inputName];
}

QString SecMeasInputContainer::getAlias(QString inputName) const
{
    Q_ASSERT(m_aliasHash.contains(inputName));
    return m_aliasHash[inputName];
}

QString SecMeasInputContainer::getInputNameFromDisplayedName(QString displayName) const
{
    Q_ASSERT(m_displayHash.contains(displayName));
    return m_displayHash[displayName];
}

void SecMeasInputContainer::setNotificationId(QString inputName, int notificationId)
{
    Q_ASSERT(!m_notificationIdHash.contains(notificationId));
    m_notificationIdHash[notificationId] = inputName;
}

QString SecMeasInputContainer::getInputNameFromNotificationId(int notificationId)
{
    Q_ASSERT(m_notificationIdHash.contains(notificationId));
    return m_notificationIdHash[notificationId];
}

QStringList SecMeasInputContainer::getInputNameList()
{
    return m_resourceHash.keys();
}

void SecMeasInputContainer::setCurrentInput(QString currentRefIn)
{
    Q_ASSERT(m_resourceHash.contains(currentRefIn));
    m_currentInput = currentRefIn;
}

QString SecMeasInputContainer::getCurrentInput() const
{
    return m_currentInput;
}
