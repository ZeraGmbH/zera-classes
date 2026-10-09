#include "semspmmoduleconfiguration.h"

SemSpmModuleConfiguration::SemSpmModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setRefInputCount,

    setRefInput,
    setTargeted,
    setMeasTime,
    setUpperLimit,
    setLowerLimit,

    setRefInput1Name = 32,
};

void SemSpmModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_ConfigXMLMap["confpar:configuration:measure:refinput:n"] = setRefInputCount;

    m_ConfigXMLMap["confpar:parameter:measure:refinput"] = setRefInput;
    m_ConfigXMLMap["confpar:parameter:measure:targeted"] = setTargeted;
    m_ConfigXMLMap["confpar:parameter:measure:meastime"] = setMeasTime;
    m_ConfigXMLMap["confpar:parameter:measure:upperlimit"] = setUpperLimit;
    m_ConfigXMLMap["confpar:parameter:measure:lowerlimit"] = setLowerLimit;

    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &SemSpmModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &SemSpmModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}


QByteArray SemSpmModuleConfiguration::exportConfiguration() const
{
    const stringParameter* paramRefInput = &m_configData.m_refConfigs.m_sRefInput;
    m_pXMLReader->setValue(paramRefInput->m_sKey, paramRefInput->m_sValue);

    const boolParameter* paramTargeted = &m_configData.m_bTargeted;
    m_pXMLReader->setValue(paramTargeted->m_sKey, QString("%1").arg(paramTargeted->m_nActive));

    const intParameter* paramMeasTime = &m_configData.m_nMeasTime;
    m_pXMLReader->setValue(paramMeasTime->m_sKey, QString("%1").arg(paramMeasTime->m_nValue));

    const doubleParameter* paramUpperLimit = &m_configData.m_limitConfigs.m_fUpperLimit;
    m_pXMLReader->setValue(paramUpperLimit->m_sKey, QString("%1").arg(paramUpperLimit->m_fValue));

    const doubleParameter* paramLowerLimit = &m_configData.m_limitConfigs.m_fLowerLimit;
    m_pXMLReader->setValue(paramLowerLimit->m_sKey, QString("%1").arg(paramLowerLimit->m_fValue));

    return m_pXMLReader->getXMLConfig().toUtf8();
}

SemSpmModuleConfigData *SemSpmModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void SemSpmModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setRefInputCount:
            m_configData.m_refConfigs.m_nRefInpCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_configData.m_refConfigs.m_nRefInpCount; i++) {
                m_ConfigXMLMap[QString("confpar:configuration:measure:refinput:inp%1").arg(i+1)] = setRefInput1Name+i;
                m_configData.m_refConfigs.m_refInpList.append(TRefInput());
            }
            break;
        case setRefInput:
            m_configData.m_refConfigs.m_sRefInput.m_sKey = key;
            m_configData.m_refConfigs.m_sRefInput.m_sValue = m_pXMLReader->getValue(key);
            break;
        case setTargeted:
            m_configData.m_bTargeted.m_sKey = key;
            m_configData.m_bTargeted.m_nActive = m_pXMLReader->getValue(key).toInt();
            break;
        case setMeasTime:
            m_configData.m_nMeasTime.m_sKey = key;
            m_configData.m_nMeasTime.m_nValue = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setUpperLimit:
            m_configData.m_limitConfigs.m_fUpperLimit.m_sKey = key;
            m_configData.m_limitConfigs.m_fUpperLimit.m_fValue = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setLowerLimit:
            m_configData.m_limitConfigs.m_fLowerLimit.m_sKey = key;
            m_configData.m_limitConfigs.m_fLowerLimit.m_fValue = m_pXMLReader->getValue(key).toDouble(&ok);
            break;

        default:
            if ((cmd >= setRefInput1Name) && (cmd < setRefInput1Name + 32)) {
                cmd -= setRefInput1Name;
                const QStringList refInputFNameAndAlias = m_pXMLReader->getValue(key).split(",");
                if (refInputFNameAndAlias.count() != 2) {
                    qCritical("SPM configuration: Input is not comma separated input,alias!");
                    m_bConfigError = true;
                }
                else {
                    TRefInput refInput;
                    refInput.inputName = refInputFNameAndAlias[0];
                    refInput.alias = refInputFNameAndAlias[1];
                    m_configData.m_refConfigs.m_refInpList.replace(cmd, refInput);
                }
            }
            break;
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void SemSpmModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}
