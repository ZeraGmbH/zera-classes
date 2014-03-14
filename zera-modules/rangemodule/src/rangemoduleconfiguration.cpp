#include <QPoint>
#include <xmlconfigreader.h>

#include "rangemoduleconfiguration.h"
#include "rangemoduleconfigdata.h"
#include "socket.h"


cRangeModuleConfiguration::cRangeModuleConfiguration()
{
}


cRangeModuleConfiguration::~cRangeModuleConfiguration()
{
    if (m_pRangeModulConfigData) delete m_pRangeModulConfigData;
}


void cRangeModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pRangeModulConfigData) delete m_pRangeModulConfigData;
    m_pRangeModulConfigData = new cRangeModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["rangemodconfpar:configuration:connectivity:debugLevel"] = setDebugLevel;
    m_ConfigXMLMap["rangemodconfpar:configuration:connectivity:ethernet:resourcemanager:ip"] = setRMIp;
    m_ConfigXMLMap["rangemodconfpar:configuration:connectivity:ethernet:resourcemanager:port"] = setRMPort;
    m_ConfigXMLMap["rangemodconfpar:configuration:connectivity:ethernet:pcbserver:ip"] = setPCBServerIp;
    m_ConfigXMLMap["rangemodconfpar:configuration:connectivity:ethernet:pcbserver:port"] = setPCBServerPort;
    m_ConfigXMLMap["rangemodconfpar:configuration:connectivity:ethernet:pcbserver:ip"] = setDSPServerIp;
    m_ConfigXMLMap["rangemodconfpar:configuration:connectivity:ethernet:pcbserver:port"] = setDSPServerPort;

    m_ConfigXMLMap["rangemodconfpar:configuration:sense:channel:n"] = setChannelCount;
    m_ConfigXMLMap["rangemodconfpar:configuration:sense:group:n"] = setGroupCount;

    m_ConfigXMLMap["rangemodconfpar:parameter:sense:group"] = setGrouping;
    m_ConfigXMLMap["rangemodconfpar:parameter:sense:auto"] = setAutomatic;
    m_ConfigXMLMap["rangemodconfpar:parameter:measure:interval"] = setInterval;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
    {
        connect(m_pXMLReader, SIGNAL(valueChanged(const QString&)), this, SLOT(configXMLInfo(const QString&)));
        connect(m_pXMLReader, SIGNAL(finishedParsingXML(bool)), this, SLOT(completeConfiguration(bool)));
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    }
    else
        m_bConfigError = true;
}


QByteArray cRangeModuleConfiguration::exportConfiguration()
{
//    QList<QString> keyList = m_exportEntityList.keys();
//    for (int i = 0; i << keyList.count(); i++)
//        m_pXMLReader->setValue(keyList.at(i), m_exportEntityList[keyList.at(i)]);

    return m_pXMLReader->getXMLConfig().toUtf8();
}


cRangeModuleConfigData *cRangeModuleConfiguration::getConfigurationData()
{
    return m_pRangeModulConfigData;
}


void cRangeModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;
    int n;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDebugLevel:
            m_pRangeModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setRMIp:
            m_pRangeModulConfigData->m_RMSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setRMPort:
            m_pRangeModulConfigData->m_RMSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPCBServerIp:
            m_pRangeModulConfigData->m_PCBServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setPCBServerPort:
            m_pRangeModulConfigData->m_PCBServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setDSPServerIp:
            m_pRangeModulConfigData->m_DSPServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setDSPServerPort:
            m_pRangeModulConfigData->m_DSPServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setChannelCount:
            m_pRangeModulConfigData->m_nChannelCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for channel configuration
            for (int i = 0; i < m_pRangeModulConfigData->m_nChannelCount; i++)
                m_ConfigXMLMap[QString("rangemodconfpar:configuration:sense:channel:ch%1").arg(i+1)] = setSenseChannel1+i;
            break;
        case setGroupCount:
            m_pRangeModulConfigData->m_nGroupCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for group configuration
            for (int i = 0; i < m_pRangeModulConfigData->m_nGroupCount; i++)
            {
                 m_ConfigXMLMap[QString("rangemodconfpar:configuration:sense:group:gr%1:n").arg(i+1)] = setGroup1ChannelCount+i;
                 m_pRangeModulConfigData->m_GroupCountList.append(n=0); // and the needed lists
                 m_pRangeModulConfigData->m_GroupList.append(QStringList());
            }
            break;
        case setGrouping:
            m_pRangeModulConfigData->m_nGroupAct = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setAutomatic:
            m_pRangeModulConfigData->m_nAutoAct = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setInterval:
            m_pRangeModulConfigData->m_fInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        default:
            if ((cmd >= setSenseChannel1) && (cmd < setSenseChannel1 + m_pRangeModulConfigData->m_nChannelCount))
            {
                // it is command for setting channel name
                m_pRangeModulConfigData->m_senseChannelList.append(m_pXMLReader->getValue(key)); // for configuration of our engine
            }

            else

            if ((cmd >= setGroup1ChannelCount) && (cmd < (setGroup1ChannelCount+ m_pRangeModulConfigData->m_nGroupCount)))
            {   // it is cmd for setting groupx channel count
                cmd -= setGroup1ChannelCount;
                m_pRangeModulConfigData->m_GroupCountList.replace(cmd , m_pXMLReader->getValue(key).toInt(&ok));
                for (int i = 0; i < m_pRangeModulConfigData->m_GroupCountList.at(cmd); i++) // create hash entries for setting channels in this group
                    m_ConfigXMLMap[QString("rangemodconfpar:configuration:sense:group:gr%1:ch%2").arg(cmd+1).arg(i+1)] = setGroup1Channel1 +(cmd << 5) +i;
            }

            else

            for (int i = 0; i < m_pRangeModulConfigData->m_nGroupCount; i++)
                if ((cmd >= setGroup1Channel1 + (i << 5)) && (cmd < (setGroup1Channel1+ (i << 5) + 32)))
                {
                    QStringList sl = m_pRangeModulConfigData->m_GroupList.at(i);
                    sl.append(m_pXMLReader->getValue(key));
                }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cRangeModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}



