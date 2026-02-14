#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "apimoduleconfiguration.h"
#include "apimoduleconfigdata.h"

namespace APIMODULE
{
    cApiModuleConfiguration::cApiModuleConfiguration()
    {
        connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cApiModuleConfiguration::configXMLInfo);
        connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cApiModuleConfiguration::completeConfiguration);
    }

    cApiModuleConfiguration::~cApiModuleConfiguration()
    {
        if (m_pApiModulConfigData)
            delete m_pApiModulConfigData;
    }

    void cApiModuleConfiguration::setConfiguration(const QByteArray& xmlString)
    {
        m_bConfigured = m_bConfigError = false;
        if (m_pApiModulConfigData)
            delete m_pApiModulConfigData;
        m_pApiModulConfigData = new cApiModuleConfigData();

        m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

        // so now we can set up
        // initializing hash table for xml configuration

        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    }

    QByteArray cApiModuleConfiguration::exportConfiguration()
    {
        return m_pXMLReader->getXMLConfig().toUtf8();
    }

    cApiModuleConfigData *cApiModuleConfiguration::getConfigurationData()
    {
        return m_pApiModulConfigData;
    }

    void cApiModuleConfiguration::configXMLInfo(const QString &key)
    {
        if (m_ConfigXMLMap.contains(key))
        {
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
