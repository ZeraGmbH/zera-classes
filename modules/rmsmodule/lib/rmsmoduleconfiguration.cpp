#include "rmsmoduleconfiguration.h"

namespace RMSMODULE
{

cRmsModuleConfiguration::cRmsModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setValueCount,
    setIntegrationMode,
    setMovingwindowBool,
    setMovingwindowTime,
    setMeasureIntervalTime,
    setMeasureIntervalPeriod,

    setValue1 = 20
};

void cRmsModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_ConfigXMLMap["rmsmodconfpar:configuration:measure:values:n"] = setValueCount;
    m_ConfigXMLMap["rmsmodconfpar:configuration:measure:integrationmode"] = setIntegrationMode;
    m_ConfigXMLMap["rmsmodconfpar:configuration:measure:movingwindow:on"] = setMovingwindowBool;
    m_ConfigXMLMap["rmsmodconfpar:configuration:measure:movingwindow:time"] = setMovingwindowTime;

    m_ConfigXMLMap["rmsmodconfpar:parameter:interval:time"] = setMeasureIntervalTime;
    m_ConfigXMLMap["rmsmodconfpar:parameter:interval:period"] = setMeasureIntervalPeriod;

    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cRmsModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cRmsModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cRmsModuleConfiguration::exportConfiguration() const
{
    const doubleParameter* paramInteral = &m_configData.m_fMeasIntervalTime;
    m_pXMLReader->setValue(paramInteral->m_sKey, QString("%1").arg(paramInteral->m_fValue));

    const intParameter* paramPeriod = &m_configData.m_nMeasIntervalPeriod;
    m_pXMLReader->setValue(paramPeriod->m_sKey, QString("%1").arg(paramPeriod->m_nValue));

    return m_pXMLReader->getXMLConfig().toUtf8();
}

cRmsModuleConfigData *cRmsModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cRmsModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setValueCount:
            m_configData.m_nValueCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_configData.m_nValueCount; i++)
                m_ConfigXMLMap[QString("rmsmodconfpar:configuration:measure:values:val%1").arg(i+1)] = setValue1+i;
            break;
        case setIntegrationMode:
            m_configData.m_sIntegrationMode = m_pXMLReader->getValue(key);
            break;
        case setMovingwindowBool:
            m_configData.m_bmovingWindow = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setMovingwindowTime:
            m_configData.m_fmovingwindowInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setMeasureIntervalTime:
            m_configData.m_fMeasIntervalTime.m_sKey = key;
            m_configData.m_fMeasIntervalTime.m_fValue = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setMeasureIntervalPeriod:
            m_configData.m_nMeasIntervalPeriod.m_sKey = key;
            m_configData.m_nMeasIntervalPeriod.m_nValue = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        default:
            if ((cmd >= setValue1) && (cmd < setValue1 + 32))
            {
                cmd -= setValue1;
                // it is command for setting value channel name
                QString valueChannel = m_pXMLReader->getValue(key);
                m_configData.m_valueChannelList.append(valueChannel); // for configuration of our engine
            }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cRmsModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}

