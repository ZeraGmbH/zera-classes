#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "referencemoduleconfiguration.h"
#include "referencemoduleconfigdata.h"
#include "socket.h"

namespace REFERENCEMODULE
{
static const char* defaultXSDFile = "://referencemodule.xsd";

cReferenceModuleConfiguration::cReferenceModuleConfiguration()
{
    m_pReferenceModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cReferenceModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cReferenceModuleConfiguration::completeConfiguration);
}


cReferenceModuleConfiguration::~cReferenceModuleConfiguration()
{
    if (m_pReferenceModulConfigData) delete m_pReferenceModulConfigData;
}


void cReferenceModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pReferenceModulConfigData) delete m_pReferenceModulConfigData;
    m_pReferenceModulConfigData = new cReferenceModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["referencemodconfpar:configuration:connectivity:debuglevel"] = setDebugLevel;
    m_ConfigXMLMap["referencemodconfpar:configuration:connectivity:ethernet:resourcemanager:ip"] = setRMIp;
    m_ConfigXMLMap["referencemodconfpar:configuration:connectivity:ethernet:resourcemanager:port"] = setRMPort;
    m_ConfigXMLMap["referencemodconfpar:configuration:connectivity:ethernet:pcbserver:ip"] = setPCBServerIp;
    m_ConfigXMLMap["referencemodconfpar:configuration:connectivity:ethernet:pcbserver:port"] = setPCBServerPort;
    m_ConfigXMLMap["referencemodconfpar:configuration:connectivity:ethernet:dspserver:ip"] = setDSPServerIp;
    m_ConfigXMLMap["referencemodconfpar:configuration:connectivity:ethernet:dspserver:port"] = setDSPServerPort;


    m_ConfigXMLMap["referencemodconfpar:configuration:sense:channel:n"] = setChannelCount;
    m_ConfigXMLMap["referencemodconfpar:configuration:measure:interval"] = setMeasureInterval;
    m_ConfigXMLMap["referencemodconfpar:configuration:measure:ignore"] = setMeasureIgnore;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cReferenceModuleConfiguration::exportConfiguration()
{
    // nothing to set befor export
    return m_pXMLReader->getXMLConfig().toUtf8();
}


cReferenceModuleConfigData *cReferenceModuleConfiguration::getConfigurationData()
{
    return m_pReferenceModulConfigData;
}


void cReferenceModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDebugLevel:
            m_pReferenceModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setRMIp:
            m_pReferenceModulConfigData->m_RMSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setRMPort:
            m_pReferenceModulConfigData->m_RMSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPCBServerIp:
            m_pReferenceModulConfigData->m_PCBServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setPCBServerPort:
            m_pReferenceModulConfigData->m_PCBServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setDSPServerIp:
            m_pReferenceModulConfigData->m_DSPServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setDSPServerPort:
            m_pReferenceModulConfigData->m_DSPServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setChannelCount:
        {
            m_pReferenceModulConfigData->m_nChannelCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for channel configuration
            for (int i = 0; i < m_pReferenceModulConfigData->m_nChannelCount; i++)
            {
                m_ConfigXMLMap[QString("referencemodconfpar:configuration:sense:channel:ch%1").arg(i+1)] = setReferenceChannel1+i;
            }
            break;
        }

        case setMeasureInterval:
            m_pReferenceModulConfigData->m_fMeasInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setMeasureIgnore:
            m_pReferenceModulConfigData->m_nIgnore = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        default:
            if ((cmd >= setReferenceChannel1) && (cmd < setReferenceChannel1 + m_pReferenceModulConfigData->m_nChannelCount))
            {
                cmd -= setReferenceChannel1;
                // it is command for setting channel name
                QString referenceChannel = m_pXMLReader->getValue(key);
                m_pReferenceModulConfigData->m_referenceChannelList.append(referenceChannel); // for configuration of our engine
            }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cReferenceModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

}

