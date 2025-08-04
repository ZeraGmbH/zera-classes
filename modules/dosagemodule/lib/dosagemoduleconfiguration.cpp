#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "dosagemoduleconfiguration.h"
#include "dosagemoduleconfigdata.h"

static void initResource()
{
    Q_INIT_RESOURCE(dosagemodulexmlschema);
}

namespace DOSAGEMODULE
{
static const char *defaultXSDFile = "://dosagemodule.xsd";

cDosageModuleConfiguration::cDosageModuleConfiguration()
{
    initResource();
    m_pDosageModulConfigData = nullptr;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cDosageModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cDosageModuleConfiguration::completeConfiguration);
}

cDosageModuleConfiguration::~cDosageModuleConfiguration()
{
    if (m_pDosageModulConfigData)
        delete m_pDosageModulConfigData;
}

void cDosageModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;
    if (m_pDosageModulConfigData)
        delete m_pDosageModulConfigData;
    m_pDosageModulConfigData = new cDosageModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}

QByteArray cDosageModuleConfiguration::exportConfiguration()
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}

cDosageModuleConfigData *cDosageModuleConfiguration::getConfigurationData()
{
    return m_pDosageModulConfigData;
}

void cDosageModuleConfiguration::configXMLInfo(QString key)
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

void cDosageModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}
