#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "oscimoduleconfiguration.h"
#include "oscimoduleconfigdata.h"
#include "socket.h"

namespace OSCIMODULE
{

cOsciModuleConfiguration::cOsciModuleConfiguration()
{
    m_pOsciModulConfigData = 0;
    connect(m_pXMLReader, SIGNAL(valueChanged(const QString&)), this, SLOT(configXMLInfo(const QString&)));
    connect(m_pXMLReader, SIGNAL(finishedParsingXML(bool)), this, SLOT(completeConfiguration(bool)));
}


cOsciModuleConfiguration::~cOsciModuleConfiguration()
{
    if (m_pOsciModulConfigData) delete m_pOsciModulConfigData;
}


void cOsciModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pOsciModulConfigData) delete m_pOsciModulConfigData;
    m_pOsciModulConfigData = new cOsciModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["oscimodconfpar:configuration:connectivity:debuglevel"] = setDebugLevel;
    m_ConfigXMLMap["oscimodconfpar:configuration:connectivity:ethernet:resourcemanager:ip"] = setRMIp;
    m_ConfigXMLMap["oscimodconfpar:configuration:connectivity:ethernet:resourcemanager:port"] = setRMPort;
    m_ConfigXMLMap["oscimodconfpar:configuration:connectivity:ethernet:pcbserver:ip"] = setPCBServerIp;
    m_ConfigXMLMap["oscimodconfpar:configuration:connectivity:ethernet:pcbserver:port"] = setPCBServerPort;
    m_ConfigXMLMap["oscimodconfpar:configuration:connectivity:ethernet:dspserver:ip"] = setDSPServerIp;
    m_ConfigXMLMap["oscimodconfpar:configuration:connectivity:ethernet:dspserver:port"] = setDSPServerPort;

    m_ConfigXMLMap["oscimodconfpar:configuration:measure:interpolation"] = setInterpolation;
    m_ConfigXMLMap["oscimodconfpar:configuration:measure:gap"] = setGap;
    m_ConfigXMLMap["oscimodconfpar:configuration:measure:values:n"] = setValueCount;

    m_ConfigXMLMap["oscimodconfpar:parameter:refchannel"] = setRefChannel;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cOsciModuleConfiguration::exportConfiguration()
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}


cOsciModuleConfigData *cOsciModuleConfiguration::getConfigurationData()
{
    return m_pOsciModulConfigData;
}


void cOsciModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDebugLevel:
            m_pOsciModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setRMIp:
            m_pOsciModulConfigData->m_RMSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setRMPort:
            m_pOsciModulConfigData->m_RMSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPCBServerIp:
            m_pOsciModulConfigData->m_PCBServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setPCBServerPort:
            m_pOsciModulConfigData->m_PCBServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setDSPServerIp:
            m_pOsciModulConfigData->m_DSPServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setDSPServerPort:
            m_pOsciModulConfigData->m_DSPServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setInterpolation:
            m_pOsciModulConfigData->m_nInterpolation = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setGap:
            m_pOsciModulConfigData->m_nGap = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setValueCount:
            m_pOsciModulConfigData->m_nValueCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_pOsciModulConfigData->m_nValueCount; i++)
                m_ConfigXMLMap[QString("oscimodconfpar:configuration:measure:values:val%1").arg(i+1)] = setValue1+i;
            break;
        case setRefChannel:
            m_pOsciModulConfigData->m_RefChannel.m_sKey = key;
            m_pOsciModulConfigData->m_RefChannel.m_sPar = m_pXMLReader->getValue(key);
            break;
        default:
            if ((cmd >= setValue1) && (cmd < setValue1 + 32))
            {
                cmd -= setValue1;
                // it is command for setting value channel name
                QString valueChannel = m_pXMLReader->getValue(key);
                m_pOsciModulConfigData->m_valueChannelList.append(valueChannel); // for configuration of our engine
            }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cOsciModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

}

