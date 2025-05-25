#include "rmsmoduleconfiguration.h"
#include "rmsmoduleconfigdata.h"
#include <xmlconfigreader.h>

namespace RMSMODULE
{
cRmsModuleConfiguration::cRmsModuleConfiguration()
{
    m_pRmsModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cRmsModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cRmsModuleConfiguration::completeConfiguration);
}


cRmsModuleConfiguration::~cRmsModuleConfiguration()
{
    if (m_pRmsModulConfigData) delete m_pRmsModulConfigData;
}


void cRmsModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pRmsModulConfigData) delete m_pRmsModulConfigData;
    m_pRmsModulConfigData = new cRmsModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["rmsmodconfpar:configuration:measure:values:n"] = setValueCount;
    m_ConfigXMLMap["rmsmodconfpar:configuration:measure:integrationmode"] = setIntegrationMode;
    m_ConfigXMLMap["rmsmodconfpar:configuration:measure:movingwindow:on"] = setMovingwindowBool;
    m_ConfigXMLMap["rmsmodconfpar:configuration:measure:movingwindow:time"] = setMovingwindowTime;

    m_ConfigXMLMap["rmsmodconfpar:parameter:interval:time"] = setMeasureIntervalTime;
    m_ConfigXMLMap["rmsmodconfpar:parameter:interval:period"] = setMeasureIntervalPeriod;

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}


QByteArray cRmsModuleConfiguration::exportConfiguration()
{
    doubleParameter* dPar;
    dPar = &m_pRmsModulConfigData->m_fMeasIntervalTime;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fValue));
    intParameter* iPar;
    iPar = &m_pRmsModulConfigData->m_nMeasIntervalPeriod;
    m_pXMLReader->setValue(iPar->m_sKey, QString("%1").arg(iPar->m_nValue));
    return m_pXMLReader->getXMLConfig().toUtf8();
}


cRmsModuleConfigData *cRmsModuleConfiguration::getConfigurationData()
{
    return m_pRmsModulConfigData;
}


void cRmsModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setValueCount:
            m_pRmsModulConfigData->m_nValueCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_pRmsModulConfigData->m_nValueCount; i++)
                m_ConfigXMLMap[QString("rmsmodconfpar:configuration:measure:values:val%1").arg(i+1)] = setValue1+i;
            break;
        case setIntegrationMode:
            m_pRmsModulConfigData->m_sIntegrationMode = m_pXMLReader->getValue(key);
            break;
        case setMovingwindowBool:
            m_pRmsModulConfigData->m_bmovingWindow = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setMovingwindowTime:
            m_pRmsModulConfigData->m_fmovingwindowInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setMeasureIntervalTime:
            m_pRmsModulConfigData->m_fMeasIntervalTime.m_sKey = key;
            m_pRmsModulConfigData->m_fMeasIntervalTime.m_fValue = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setMeasureIntervalPeriod:
            m_pRmsModulConfigData->m_nMeasIntervalPeriod.m_sKey = key;
            m_pRmsModulConfigData->m_nMeasIntervalPeriod.m_nValue = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        default:
            if ((cmd >= setValue1) && (cmd < setValue1 + 32))
            {
                cmd -= setValue1;
                // it is command for setting value channel name
                QString valueChannel = m_pXMLReader->getValue(key);
                m_pRmsModulConfigData->m_valueChannelList.append(valueChannel); // for configuration of our engine
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

