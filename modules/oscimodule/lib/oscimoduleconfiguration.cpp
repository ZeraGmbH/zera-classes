#include "oscimoduleconfiguration.h"
#include "oscimoduleconfigdata.h"
#include <xmlconfigreader.h>

namespace OSCIMODULE
{
cOsciModuleConfiguration::cOsciModuleConfiguration()
{
    m_pOsciModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cOsciModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cOsciModuleConfiguration::completeConfiguration);
}


cOsciModuleConfiguration::~cOsciModuleConfiguration()
{
    if (m_pOsciModulConfigData) delete m_pOsciModulConfigData;
}


void cOsciModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pOsciModulConfigData) delete m_pOsciModulConfigData;
    m_pOsciModulConfigData = new cOsciModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["oscimodconfpar:configuration:measure:interpolation"] = setInterpolation;
    m_ConfigXMLMap["oscimodconfpar:configuration:measure:gap"] = setGap;
    m_ConfigXMLMap["oscimodconfpar:configuration:measure:values:n"] = setValueCount;

    m_ConfigXMLMap["oscimodconfpar:parameter:refchannel"] = setRefChannel;

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}


QByteArray cOsciModuleConfiguration::exportConfiguration()
{
    stringParameter sPar;
    sPar = m_pOsciModulConfigData->m_RefChannel;
    m_pXMLReader->setValue(sPar.m_sKey, QString("%1").arg(sPar.m_sPar));

    return m_pXMLReader->getXMLConfig().toUtf8();
}


cOsciModuleConfigData *cOsciModuleConfiguration::getConfigurationData()
{
    return m_pOsciModulConfigData;
}


void cOsciModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setInterpolation:
            m_pOsciModulConfigData->m_nInterpolation = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setGap:
            m_pOsciModulConfigData->m_nGap = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setValueCount:
            m_pOsciModulConfigData->m_nValueCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_pOsciModulConfigData->m_nValueCount; i++)
                m_ConfigXMLMap[QString("oscimodconfpar:configuration:measure:values:val%1").arg(i+1)] = setValue1+i;
            break;
        case setRefChannel:
            m_pOsciModulConfigData->m_RefChannel.m_sKey = key;
            m_pOsciModulConfigData->m_RefChannel.m_sPar = m_pXMLReader->getValue(key);
            break;
        default:
            if ((cmd >= setValue1) && (cmd < setValue1 + 32))
            {
                cmd -= setValue1;
                // it is command for setting value channel name
                QString valueChannel = m_pXMLReader->getValue(key);
                m_pOsciModulConfigData->m_valueChannelList.append(valueChannel); // for configuration of our engine
            }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cOsciModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}

