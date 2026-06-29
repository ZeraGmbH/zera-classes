#include "dspsupermoduleconfiguration.h"

namespace DSPSUPERMODULE
{

DspSuperModuleConfiguration::DspSuperModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setDspPeriodsArraySize,
    setPeriodsTotal,

    setValue1 = 20
};

void DspSuperModuleConfiguration::setConfiguration(const QByteArray &xmlString)
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &DspSuperModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &DspSuperModuleConfiguration::completeConfiguration);

    m_ConfigXMLMap["dspsupermodconfpar:configuration:dspperiodarraysize"] = setDspPeriodsArraySize;
    m_ConfigXMLMap["dspsupermodconfpar:configuration:periodstotal"] = setPeriodsTotal;

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray DspSuperModuleConfiguration::exportConfiguration() const
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}

DspSuperModuleConfigData *DspSuperModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void DspSuperModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd) {
        case setDspPeriodsArraySize:
            m_configData.m_dspArrayEntrySize = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPeriodsTotal:
            m_configData.m_periodsTotal = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        default:
            break;
        }
        m_bConfigError |= !ok;
    }
    else
        m_bConfigError = true;
}

void DspSuperModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}

