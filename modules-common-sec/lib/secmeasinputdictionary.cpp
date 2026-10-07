#include "secmeasinputdictionary.h"

void SecMeasInputDictionary::fillFromReferenceConfig(const TSecCommonReferenceConfigs &refConfig)
{
    const QList<TRefInput> refInputList = refConfig.m_refInpList;
    for(const TRefInput &refInput : refInputList)
        setAlias(refInput.inputName, refInput.alias);

    m_resourceTypeList.addTypesFromConfig(refConfig);
}

void SecMeasInputDictionary::addReferenceInput(const QString &inputName, const QString &resource)
{
    Q_ASSERT(!m_resourceHash.contains(inputName));
    m_resourceHash[inputName] = resource;
}

void SecMeasInputDictionary::setAlias(const QString &inputName, const QString &alias)
{
    Q_ASSERT(!m_aliasMap.contains(inputName));
    m_aliasMap[inputName] = alias;
    Q_ASSERT(!m_reverseAliasHash.contains(alias));
    m_reverseAliasHash[alias] = inputName;
}

QString SecMeasInputDictionary::getResource(const QString &inputName) const
{
    Q_ASSERT(m_resourceHash.contains(inputName));
    return m_resourceHash[inputName];
}

const QStringList &SecMeasInputDictionary::getResourceTypeList() const
{
    return m_resourceTypeList.getResourceTypeList();
}

QString SecMeasInputDictionary::getAlias(const QString &inputName) const
{
    if(m_aliasMap.contains(inputName))
        return m_aliasMap[inputName];
    qWarning("Alias for input name %s not found", qPrintable(inputName));
    return QString("P");
}

void SecMeasInputDictionary::setNotificationId(const QString &inputName, int notificationId)
{
    Q_ASSERT(!m_notificationIdMap.contains(notificationId));
    m_notificationIdMap[notificationId] = inputName;
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
