#include "statusmoduleconfiguration.h"

namespace STATUSMODULE
{

enum moduleconfigstate
{
    setMeasStatus,
    setAccumulator,
    setVeinUpdateMs
};

cStatusModuleConfiguration::cStatusModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

void cStatusModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_ConfigXMLMap["statusmodconfpar:configuration:accumulator"] = setAccumulator;
    m_ConfigXMLMap["statusmodconfpar:configuration:veinupdatems"] = setVeinUpdateMs;

    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cStatusModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cStatusModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cStatusModuleConfiguration::exportConfiguration() const
{
    // nothing to set befor export
    return m_pXMLReader->getXMLConfig().toUtf8();
}

cStatusModuleConfigData *cStatusModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cStatusModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setAccumulator:
            m_configData.m_accumulator = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setVeinUpdateMs:
            m_configData.m_veinUpdateMs = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        }

        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cStatusModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}
