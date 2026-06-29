#include "oscimoduleconfiguration.h"

namespace OSCIMODULE
{

cOsciModuleConfiguration::cOsciModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setInterpolation,
    setGap,
    setValueCount,
    setRefChannel,

    setValue1 = 20
};

void cOsciModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cOsciModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cOsciModuleConfiguration::completeConfiguration);

    m_ConfigXMLMap["oscimodconfpar:configuration:measure:interpolation"] = setInterpolation;
    m_ConfigXMLMap["oscimodconfpar:configuration:measure:gap"] = setGap;
    m_ConfigXMLMap["oscimodconfpar:configuration:measure:values:n"] = setValueCount;

    m_ConfigXMLMap["oscimodconfpar:parameter:refchannel"] = setRefChannel;

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cOsciModuleConfiguration::exportConfiguration() const
{
    const stringParameter paramRefChannel = m_configData.m_RefChannel;
    m_pXMLReader->setValue(paramRefChannel.m_sKey, QString("%1").arg(paramRefChannel.m_sPar));

    return m_pXMLReader->getXMLConfig().toUtf8();
}

cOsciModuleConfigData *cOsciModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cOsciModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setInterpolation:
            m_configData.m_nInterpolation = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setGap:
            m_configData.m_nGap = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setValueCount:
            m_configData.m_nValueCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_configData.m_nValueCount; i++)
                m_ConfigXMLMap[QString("oscimodconfpar:configuration:measure:values:val%1").arg(i+1)] = setValue1+i;
            break;
        case setRefChannel:
            m_configData.m_RefChannel.m_sKey = key;
            m_configData.m_RefChannel.m_sPar = m_pXMLReader->getValue(key);
            break;
        default:
            if ((cmd >= setValue1) && (cmd < setValue1 + 32)) {
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

void cOsciModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}

