#include <QPoint>
#include "hotplugcontrolsmoduleconfiguration.h"
#include "hotplugcontrolsmoduleconfigdata.h"
#include <xmlconfigreader.h>

namespace HOTPLUGCONTROLSMODULE
{
cHotplugControlsModuleConfiguration::cHotplugControlsModuleConfiguration()
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cHotplugControlsModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cHotplugControlsModuleConfiguration::completeConfiguration);
}


cHotplugControlsModuleConfiguration::~cHotplugControlsModuleConfiguration()
{
    if (m_pHotplugControlsModuleConfigData)
        delete m_pHotplugControlsModuleConfigData;
}

void cHotplugControlsModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_bConfigured = m_bConfigError = false;
    if (m_pHotplugControlsModuleConfigData)
        delete m_pHotplugControlsModuleConfigData;
    m_pHotplugControlsModuleConfigData = new cHotplugControlsModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cHotplugControlsModuleConfiguration::exportConfiguration()
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}

cHotplugControlsModuleConfigData *cHotplugControlsModuleConfiguration::getConfigurationData()
{
    return m_pHotplugControlsModuleConfigData;
}

void cHotplugControlsModuleConfiguration::configXMLInfo(const QString &key)
{
    bool ok;
    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
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

