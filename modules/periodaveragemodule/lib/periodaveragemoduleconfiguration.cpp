#include "periodaveragemoduleconfiguration.h"

namespace PERIODAVERAGEMODULE
{

PeriodAverageModuleConfiguration::PeriodAverageModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setMaxPeriods,
    setChannelCount,
    setPeriodCount,

    setValue1 = 20
};

void PeriodAverageModuleConfiguration::setConfiguration(const QByteArray &xmlString)
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &PeriodAverageModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &PeriodAverageModuleConfiguration::completeConfiguration);

    m_ConfigXMLMap["periodicaveragemodconfpar:configuration:maxperiods"] = setMaxPeriods;
    m_ConfigXMLMap["periodicaveragemodconfpar:configuration:measure:channels:n"] = setChannelCount;
    m_ConfigXMLMap["periodicaveragemodconfpar:parameter:measperiods"] = setPeriodCount;

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray PeriodAverageModuleConfiguration::exportConfiguration() const
{
    const intParameter *iPar = &m_configData.m_periodCount;
    m_pXMLReader->setValue(iPar->m_sKey, QString("%1").arg(iPar->m_nValue));

    return m_pXMLReader->getXMLConfig().toUtf8();
}

PeriodAverageModuleConfigData *PeriodAverageModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void PeriodAverageModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd) {
        case setMaxPeriods:
            m_configData.m_maxPeriods = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setChannelCount:
            m_configData.m_channelCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_configData.m_channelCount; i++)
                m_ConfigXMLMap[QString("periodicaveragemodconfpar:configuration:measure:channels:ch%1").arg(i+1)] = setValue1+i;
            break;
        case setPeriodCount: {
            m_configData.m_periodCount.m_sKey = key;
            int periodCount = m_pXMLReader->getValue(key).toInt(&ok);
            m_configData.m_periodCount.m_nValue = periodCount;
            int maxPeriodCount = m_configData.m_maxPeriods;
            if (periodCount > maxPeriodCount) {
                qWarning("Period count %i is larger than max period count %i!",
                         periodCount, maxPeriodCount);
                ok = false;
            }
            break;
        }
        default:
            if ((cmd >= setValue1) && (cmd < setValue1 + 20)) {
                //cmd -= setValue1;
                QString channelMName = m_pXMLReader->getValue(key);
                m_configData.m_valueChannelList.append(channelMName);
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

