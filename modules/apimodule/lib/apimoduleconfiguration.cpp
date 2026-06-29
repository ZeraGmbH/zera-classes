#include "apimoduleconfiguration.h"

namespace APIMODULE
{

cApiModuleConfiguration::cApiModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

void cApiModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cApiModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cApiModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cApiModuleConfiguration::exportConfiguration() const
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}

cApiModuleConfigData *cApiModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cApiModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        }
        m_bConfigError |= !ok;
    }
    else
        m_bConfigError = true;
}

void cApiModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}
