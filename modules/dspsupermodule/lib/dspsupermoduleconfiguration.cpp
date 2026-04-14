#include "dspsupermoduleconfiguration.h"
#include "dspsupermoduleconfigdata.h"
#include <xmlconfigreader.h>

namespace DSPSUPERMODULE
{

enum moduleconfigstate
{
    setDspPeriodsArraySize,
    setPeriodsTotal,

    setValue1 = 20
};

DspSuperModuleConfiguration::DspSuperModuleConfiguration()
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &DspSuperModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &DspSuperModuleConfiguration::completeConfiguration);
}

DspSuperModuleConfiguration::~DspSuperModuleConfiguration()
{
    delete m_dspSuperModulConfigData;
}

void DspSuperModuleConfiguration::setConfiguration(const QByteArray &xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_dspSuperModulConfigData)
        delete m_dspSuperModulConfigData;
    m_dspSuperModulConfigData = new DspSuperModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    m_ConfigXMLMap["dspsupermodconfpar:configuration:dspperiodarraysize"] = setDspPeriodsArraySize;
    m_ConfigXMLMap["dspsupermodconfpar:configuration:periodstotal"] = setPeriodsTotal;

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray DspSuperModuleConfiguration::exportConfiguration()
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}

DspSuperModuleConfigData *DspSuperModuleConfiguration::getConfigurationData()
{
    return m_dspSuperModulConfigData;
}

void DspSuperModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd) {
        case setDspPeriodsArraySize:
            m_dspSuperModulConfigData->m_dspArrayEntrySize = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPeriodsTotal:
            m_dspSuperModulConfigData->m_periodsTotal = m_pXMLReader->getValue(key).toInt(&ok);
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

