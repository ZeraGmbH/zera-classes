#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "statusmoduleconfiguration.h"
#include "statusmoduleconfigdata.h"
#include "socket.h"

namespace STATUSMODULE
{
static const char* defaultXSDFile = "://statusmodule.xsd";

cStatusModuleConfiguration::cStatusModuleConfiguration()
{
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

    m_ConfigXMLMap["statusmodconfpar:configuration:connectivity:debuglevel"] = setDebugLevel;
    m_ConfigXMLMap["statusmodconfpar:configuration:connectivity:ethernet:resourcemanager:ip"] = setRMIp;
    m_ConfigXMLMap["statusmodconfpar:configuration:connectivity:ethernet:resourcemanager:port"] = setRMPort;
    m_ConfigXMLMap["statusmodconfpar:configuration:connectivity:ethernet:pcbserver:ip"] = setPCBServerIp;
    m_ConfigXMLMap["statusmodconfpar:configuration:connectivity:ethernet:pcbserver:port"] = setPCBServerPort;
    m_ConfigXMLMap["statusmodconfpar:configuration:connectivity:ethernet:dspserver:ip"] = setDSPServerIp;
    m_ConfigXMLMap["statusmodconfpar:configuration:connectivity:ethernet:dspserver:port"] = setDSPServerPort;
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
        case setDebugLevel:
            m_pStatusModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setRMIp:
            m_pStatusModulConfigData->m_RMSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setRMPort:
            m_pStatusModulConfigData->m_RMSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPCBServerIp:
            m_pStatusModulConfigData->m_PCBServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setPCBServerPort:
            m_pStatusModulConfigData->m_PCBServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setDSPServerIp:
            m_pStatusModulConfigData->m_DSPServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setDSPServerPort:
            m_pStatusModulConfigData->m_DSPServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
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
    emit configXMLDone();
}

}

