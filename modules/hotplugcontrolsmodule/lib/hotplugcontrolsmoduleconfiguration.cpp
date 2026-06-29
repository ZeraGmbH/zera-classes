#include "hotplugcontrolsmoduleconfiguration.h"

namespace HOTPLUGCONTROLSMODULE
{

cHotplugControlsModuleConfiguration::cHotplugControlsModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

void cHotplugControlsModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cHotplugControlsModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cHotplugControlsModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cHotplugControlsModuleConfiguration::exportConfiguration() const
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}

cHotplugControlsModuleConfigData *cHotplugControlsModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cHotplugControlsModuleConfiguration::configXMLInfo(const QString &key)
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


void cHotplugControlsModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}

