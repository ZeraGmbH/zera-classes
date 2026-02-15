#include "periodaveragemoduleconfiguration.h"
#include "periodaveragemoduleconfigdata.h"
#include <xmlconfigreader.h>

namespace PERIODAVERAGEMODULE
{

enum moduleconfigstate
{
    setChannelCount,
    setPeriodCount,

    setValue1 = 20
};

PeriodAverageModuleConfiguration::PeriodAverageModuleConfiguration()
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &PeriodAverageModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &PeriodAverageModuleConfiguration::completeConfiguration);
}

PeriodAverageModuleConfiguration::~PeriodAverageModuleConfiguration()
{
    delete m_periodAverageModulConfigData;
}

void PeriodAverageModuleConfiguration::setConfiguration(const QByteArray &xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_periodAverageModulConfigData)
        delete m_periodAverageModulConfigData;
    m_periodAverageModulConfigData = new PeriodAverageModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    m_ConfigXMLMap["periodicaveragemodconfpar:configuration:measure:channels:n"] = setChannelCount;
    m_ConfigXMLMap["periodicaveragemodconfpar:parameter:measperiods"] = setPeriodCount;

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray PeriodAverageModuleConfiguration::exportConfiguration()
{
    intParameter *iPar = &m_periodAverageModulConfigData->m_periodCount;
    m_pXMLReader->setValue(iPar->m_sKey, QString("%1").arg(iPar->m_nValue));

    return m_pXMLReader->getXMLConfig().toUtf8();
}

PeriodAverageModuleConfigData *PeriodAverageModuleConfiguration::getConfigurationData()
{
    return m_periodAverageModulConfigData;
}

void PeriodAverageModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd) {
        case setChannelCount:
            m_periodAverageModulConfigData->m_channelCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_periodAverageModulConfigData->m_channelCount; i++)
                m_ConfigXMLMap[QString("periodicaveragemodconfpar:configuration:measure:channels:ch%1").arg(i+1)] = setValue1+i;
            break;
        case setPeriodCount:
            m_periodAverageModulConfigData->m_periodCount.m_sKey = key;
            m_periodAverageModulConfigData->m_periodCount.m_nValue = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        default:
            if ((cmd >= setValue1) && (cmd < setValue1 + 20)) {
                //cmd -= setValue1;
                QString channelMName = m_pXMLReader->getValue(key);
                m_periodAverageModulConfigData->m_valueChannelList.append(channelMName);
            }
        }
        m_bConfigError |= !ok;
    }
    else
        m_bConfigError = true;
}

void PeriodAverageModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}

