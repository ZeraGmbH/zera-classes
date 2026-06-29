#include "spm1moduleconfiguration.h"

namespace SPM1MODULE
{

cSpm1ModuleConfiguration::cSpm1ModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setRefInputCount,
    setActiveUnitCount,
    setReactiveUnitCount,
    setApparentUnitCount,

    setRefInput,
    setTargeted,
    setMeasTime,
    setUpperLimit,
    setLowerLimit,

    setRefInput1Name = 32,
    setActiveUnit1Name = 64,
    setReactiveUnit1Name = 80,
    setApparentUnit1Name = 96,
    setRefInput1Append = 128
};

void cSpm1ModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_ConfigXMLMap["spm1modconfpar:configuration:measure:refinput:n"] = setRefInputCount;
    m_ConfigXMLMap["spm1modconfpar:configuration:measure:activeunits:n"] = setActiveUnitCount;
    m_ConfigXMLMap["spm1modconfpar:configuration:measure:reactiveunits:n"] = setReactiveUnitCount;
    m_ConfigXMLMap["spm1modconfpar:configuration:measure:apparentunits:n"] = setApparentUnitCount;

    m_ConfigXMLMap["spm1modconfpar:parameter:measure:refinput"] = setRefInput;
    m_ConfigXMLMap["spm1modconfpar:parameter:measure:targeted"] = setTargeted;
    m_ConfigXMLMap["spm1modconfpar:parameter:measure:meastime"] = setMeasTime;
    m_ConfigXMLMap["spm1modconfpar:parameter:measure:upperlimit"] = setUpperLimit;
    m_ConfigXMLMap["spm1modconfpar:parameter:measure:lowerlimit"] = setLowerLimit;

    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cSpm1ModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cSpm1ModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}


QByteArray cSpm1ModuleConfiguration::exportConfiguration() const
{
    const stringParameter* paramRefInput = &m_configData.m_sRefInput;
    m_pXMLReader->setValue(paramRefInput->m_sKey, paramRefInput->m_sPar);

    const boolParameter* paramTargeted = &m_configData.m_bTargeted;
    m_pXMLReader->setValue(paramTargeted->m_sKey, QString("%1").arg(paramTargeted->m_nActive));

    const intParameter* paramMeasTime = &m_configData.m_nMeasTime;
    m_pXMLReader->setValue(paramMeasTime->m_sKey, QString("%1").arg(paramMeasTime->m_nPar));

    const doubleParameter* paramUpperLimit = &m_configData.m_fUpperLimit;
    m_pXMLReader->setValue(paramUpperLimit->m_sKey, QString("%1").arg(paramUpperLimit->m_fPar));

    const doubleParameter* paramLowerLimit = &m_configData.m_fLowerLimit;
    m_pXMLReader->setValue(paramLowerLimit->m_sKey, QString("%1").arg(paramLowerLimit->m_fPar));

    return m_pXMLReader->getXMLConfig().toUtf8();
}

cSpm1ModuleConfigData *cSpm1ModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cSpm1ModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setRefInputCount:
            m_configData.m_nRefInpCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_configData.m_nRefInpCount; i++) {
                m_ConfigXMLMap[QString("spm1modconfpar:configuration:measure:refinput:inp%1").arg(i+1)] = setRefInput1Name+i;
                m_ConfigXMLMap[QString("spm1modconfpar:configuration:measure:refinput_appends:append%1").arg(i+1)] = setRefInput1Append+i;
                m_configData.m_refInpList.append(TRefInput());
            }
            break;
        case setActiveUnitCount:
            m_configData.m_nActiveUnitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_configData.m_nActiveUnitCount; i++) {
                m_ConfigXMLMap[QString("spm1modconfpar:configuration:measure:activeunits:unit%1").arg(i+1)] = setActiveUnit1Name+i;
                m_configData.m_ActiveUnitList.append(QString());
            }
            break;
        case setReactiveUnitCount:
            m_configData.m_nReactiveUnitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_configData.m_nReactiveUnitCount; i++) {
                m_ConfigXMLMap[QString("spm1modconfpar:configuration:measure:reactiveunits:unit%1").arg(i+1)] = setReactiveUnit1Name+i;
                m_configData.m_ReactiveUnitList.append(QString());
            }
            break;
        case setApparentUnitCount:
            m_configData.m_nApparentUnitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_configData.m_nApparentUnitCount; i++) {
                m_ConfigXMLMap[QString("spm1modconfpar:configuration:measure:apparentunits:unit%1").arg(i+1)] = setApparentUnit1Name+i;
                m_configData.m_ApparentUnitList.append(QString());
            }
            break;
        case setRefInput:
            m_configData.m_sRefInput.m_sKey = key;
            m_configData.m_sRefInput.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setTargeted:
            m_configData.m_bTargeted.m_sKey = key;
            m_configData.m_bTargeted.m_nActive = m_pXMLReader->getValue(key).toInt();
            break;
        case setMeasTime:
            m_configData.m_nMeasTime.m_sKey = key;
            m_configData.m_nMeasTime.m_nPar = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setUpperLimit:
            m_configData.m_fUpperLimit.m_sKey = key;
            m_configData.m_fUpperLimit.m_fPar = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setLowerLimit:
            m_configData.m_fLowerLimit.m_sKey = key;
            m_configData.m_fLowerLimit.m_fPar = m_pXMLReader->getValue(key).toDouble(&ok);
            break;

        default:
            if ((cmd >= setRefInput1Name) && (cmd < setRefInput1Name + 32)) {
                cmd -= setRefInput1Name;
                TRefInput refInput;
                refInput.inputName = m_pXMLReader->getValue(key);
                m_configData.m_refInpList.replace(cmd, refInput);
            }
            else if ((cmd >= setActiveUnit1Name) && (cmd < setActiveUnit1Name + 16)) {
                cmd -= setActiveUnit1Name;
                QString name = m_pXMLReader->getValue(key);
                m_configData.m_ActiveUnitList.replace(cmd, name);
            }
            else if ((cmd >= setReactiveUnit1Name) && (cmd < setReactiveUnit1Name + 16)) {
                cmd -= setReactiveUnit1Name;
                QString name = m_pXMLReader->getValue(key);
                m_configData.m_ReactiveUnitList.replace(cmd, name);
            }
            else if ((cmd >= setApparentUnit1Name) && (cmd < setApparentUnit1Name + 16)) {
                cmd -= setApparentUnit1Name;
                QString name = m_pXMLReader->getValue(key);
                m_configData.m_ApparentUnitList.replace(cmd, name);
            }
            else if ((cmd >= setRefInput1Append) && (cmd < setRefInput1Append + 32)) {
                cmd -= setRefInput1Append;
                TRefInput refInput = m_configData.m_refInpList[cmd];
                refInput.nameAppend = m_pXMLReader->getValue(key);
                m_configData.m_refInpList.replace(cmd, refInput);
            }
            break;
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cSpm1ModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}

