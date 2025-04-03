#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "sfcmoduleconfiguration.h"
#include "sfcmoduleconfigdata.h"

static void initResource()
{
    Q_INIT_RESOURCE(sfcmodulexmlschema);
}

namespace SFCMODULE
{
static const char *defaultXSDFile = "://sfcmodule.xsd";

cSfcModuleConfiguration::cSfcModuleConfiguration()
{
    initResource();
    m_pSfcModulConfigData = nullptr;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cSfcModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cSfcModuleConfiguration::completeConfiguration);
}

cSfcModuleConfiguration::~cSfcModuleConfiguration()
{
    if (m_pSfcModulConfigData)
        delete m_pSfcModulConfigData;
}

void cSfcModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;
    if (m_pSfcModulConfigData)
        delete m_pSfcModulConfigData;
    m_pSfcModulConfigData = new cSfcModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash tasfc for xml configuration

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}

QByteArray cSfcModuleConfiguration::exportConfiguration()
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}

cSfcModuleConfigData *cSfcModuleConfiguration::getConfigurationData()
{
    return m_pSfcModulConfigData;
}

void cSfcModuleConfiguration::configXMLInfo(QString key)
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

void cSfcModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}
