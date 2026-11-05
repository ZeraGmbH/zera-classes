#include "secmeasinputdictionary.h"

void SecMeasInputDictionary::fillFromConfig(const TSecCommonInputConfigs &config)
{
    const QList<TInputNameAlias> inputList = config.m_inputList;
    for(const TInputNameAlias &input : inputList) {
        m_inputsOrderedByConfig.append(input.inputName);
        setAlias(input.inputName, input.alias);
    }
    m_resourceTypeList.addTypesFromConfig(config);
}

void SecMeasInputDictionary::setAlias(const QString &inputName, const QString &alias)
{
    Q_ASSERT(!m_aliasMap.contains(inputName));
    m_aliasMap[inputName] = alias;
    Q_ASSERT(!m_reverseAliasHash.contains(alias));
    m_reverseAliasHash[alias] = inputName;
}

const QStringList &SecMeasInputDictionary::getResourceTypeList() const
{
    return m_resourceTypeList.getResourceTypeList();
}

QString SecMeasInputDictionary::getAlias(const QString &inputName) const
{
    if(m_aliasMap.contains(inputName))
        return m_aliasMap[inputName];
    qCritical("Alias for input name %s not found", qPrintable(inputName));
    return QString("P");
}

QString SecMeasInputDictionary::getInputFNameFromAlias(const QString &alias) const
{
    Q_ASSERT(m_reverseAliasHash.contains(alias));
    return m_reverseAliasHash[alias];
}

const QStringList &SecMeasInputDictionary::getInputNameList() const
{
    return m_inputsOrderedByConfig;
}

QStringList SecMeasInputDictionary::getInputAliasList() const
{
    QStringList orderedAliases;
    for (const QString &input : m_inputsOrderedByConfig)
        orderedAliases.append(m_aliasMap[input]);
    return orderedAliases;
}
