#include "referencemoduleconfiguration.h"

namespace REFERENCEMODULE
{

cReferenceModuleConfiguration::cReferenceModuleConfiguration(const QByteArray& xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setChannelCount,
    setMeasureInterval,
    setMeasureIgnore,

    setReferenceChannel1 = 16 // max. 32 channels

};

void cReferenceModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_ConfigXMLMap["referencemodconfpar:configuration:sense:channel:n"] = setChannelCount;
    m_ConfigXMLMap["referencemodconfpar:configuration:measure:interval"] = setMeasureInterval;
    m_ConfigXMLMap["referencemodconfpar:configuration:measure:ignore"] = setMeasureIgnore;

    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cReferenceModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cReferenceModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cReferenceModuleConfiguration::exportConfiguration() const
{
    // nothing to set befor export
    return m_pXMLReader->getXMLConfig().toUtf8();
}

cReferenceModuleConfigData *cReferenceModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cReferenceModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setChannelCount:
        {
            m_configData.m_nChannelCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for channel configuration
            for (int i = 0; i < m_configData.m_nChannelCount; i++)
                m_ConfigXMLMap[QString("referencemodconfpar:configuration:sense:channel:ch%1").arg(i+1)] = setReferenceChannel1+i;
            break;
        }

        case setMeasureInterval:
            m_configData.m_fMeasInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setMeasureIgnore:
            m_configData.m_nIgnore = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        default:
            if ((cmd >= setReferenceChannel1) && (cmd < setReferenceChannel1 + m_configData.m_nChannelCount)) {
                cmd -= setReferenceChannel1;
                // it is command for setting channel name
                QString referenceChannel = m_pXMLReader->getValue(key);
                m_configData.m_referenceChannelList.append(referenceChannel); // for configuration of our engine
            }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cReferenceModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}

