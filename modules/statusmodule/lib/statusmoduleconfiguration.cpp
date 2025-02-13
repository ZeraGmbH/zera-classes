#include <QPoint>
#include "statusmoduleconfiguration.h"
#include "statusmoduleconfigdata.h"
#include <xmlconfigreader.h>

static void initResource()
{
    Q_INIT_RESOURCE(statusmodulexmlschema);
}

namespace STATUSMODULE
{
static const char* defaultXSDFile = "://statusmodule.xsd";

cStatusModuleConfiguration::cStatusModuleConfiguration()
{
    initResource();
    m_pStatusModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cStatusModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cStatusModuleConfiguration::completeConfiguration);
}


cStatusModuleConfiguration::~cStatusModuleConfiguration()
{
    if (m_pStatusModulConfigData) delete m_pStatusModulConfigData;
}

void cStatusModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    validateAndSetConfig(xmlString, defaultXSDFile);
}

void STATUSMODULE::cStatusModuleConfiguration::validateAndSetConfig(QByteArray xmlString, QString xsdFilename)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pStatusModulConfigData) delete m_pStatusModulConfigData;
    m_pStatusModulConfigData = new cStatusModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration
    m_ConfigXMLMap["statusmodconfpar:configuration:accumulator"] = setAccumulator;

    if (m_pXMLReader->loadSchema(xsdFilename))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}

QByteArray cStatusModuleConfiguration::exportConfiguration()
{
    // nothing to set befor export
    return m_pXMLReader->getXMLConfig().toUtf8();
}

cStatusModuleConfigData *cStatusModuleConfiguration::getConfigurationData()
{
    return m_pStatusModulConfigData;
}

void cStatusModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setAccumulator:
            m_pStatusModulConfigData->m_accumulator = m_pXMLReader->getValue(key).toInt(&ok);
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

