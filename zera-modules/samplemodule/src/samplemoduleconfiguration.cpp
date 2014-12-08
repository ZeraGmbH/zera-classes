#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "samplemoduleconfiguration.h"
#include "samplemoduleconfigdata.h"
#include "socket.h"

namespace SAMPLEMODULE
{

cSampleModuleConfiguration::cSampleModuleConfiguration()
{
    m_pSampleModulConfigData = 0;
    connect(m_pXMLReader, SIGNAL(valueChanged(const QString&)), this, SLOT(configXMLInfo(const QString&)));
    connect(m_pXMLReader, SIGNAL(finishedParsingXML(bool)), this, SLOT(completeConfiguration(bool)));
}


cSampleModuleConfiguration::~cSampleModuleConfiguration()
{
    if (m_pSampleModulConfigData) delete m_pSampleModulConfigData;
}


void cSampleModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pSampleModulConfigData) delete m_pSampleModulConfigData;
    m_pSampleModulConfigData = new cSampleModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["samplemodconfpar:configuration:connectivity:debuglevel"] = setDebugLevel;
    m_ConfigXMLMap["samplemodconfpar:configuration:connectivity:ethernet:resourcemanager:ip"] = setRMIp;
    m_ConfigXMLMap["samplemodconfpar:configuration:connectivity:ethernet:resourcemanager:port"] = setRMPort;
    m_ConfigXMLMap["samplemodconfpar:configuration:connectivity:ethernet:pcbserver:ip"] = setPCBServerIp;
    m_ConfigXMLMap["samplemodconfpar:configuration:connectivity:ethernet:pcbserver:port"] = setPCBServerPort;
    m_ConfigXMLMap["samplemodconfpar:configuration:connectivity:ethernet:dspserver:ip"] = setDSPServerIp;
    m_ConfigXMLMap["samplemodconfpar:configuration:connectivity:ethernet:dspserver:port"] = setDSPServerPort;

    m_ConfigXMLMap["samplemodconfpar:configuration:sample:system"] = setSampleSystem;
    m_ConfigXMLMap["samplemodconfpar:configuration:sample:withpllautomatic"] = setPllAutomaticBool;

    m_ConfigXMLMap["samplemodconfpar:configuration:sample:pllchannel:n"] = setPllChannelCount;
    // delegates for setting channels itself are generated dynamically

    m_ConfigXMLMap["samplemodconfpar:configuration:measure:interval"] = setMeasureInterval;

    m_ConfigXMLMap["samplemodconfpar:parameter:sample:pllauto"] = setPllAutomatic;
    m_ConfigXMLMap["samplemodconfpar:parameter:sample:pllchannel"] = setPllChannel;
    m_ConfigXMLMap["samplemodconfpar:parameter:sample:range"] = setSampleSystemRange;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cSampleModuleConfiguration::exportConfiguration()
{
//    QList<QString> keyList = m_exportEntityList.keys();
//    for (int i = 0; i << keyList.count(); i++)
//        m_pXMLReader->setValue(keyList.at(i), m_exportEntityList[keyList.at(i)]);

    boolParameter* bPar;
    bPar = &m_pSampleModulConfigData->m_ObsermaticConfPar.m_npllAutoAct;
    m_pXMLReader->setValue(bPar->m_sKey, QString("%1").arg(bPar->m_nActive));

    stringParameter* sPar;
    sPar = &m_pSampleModulConfigData->m_ObsermaticConfPar.m_pllChannel;
    m_pXMLReader->setValue(sPar->m_sKey, sPar->m_sPar);
    sPar = &m_pSampleModulConfigData->m_ObsermaticConfPar.m_pllRange;
    m_pXMLReader->setValue(sPar->m_sKey, sPar->m_sPar);

    return m_pXMLReader->getXMLConfig().toUtf8();
}


cSampleModuleConfigData *cSampleModuleConfiguration::getConfigurationData()
{
    return m_pSampleModulConfigData;
}


void cSampleModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDebugLevel:
            m_pSampleModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setRMIp:
            m_pSampleModulConfigData->m_RMSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setRMPort:
            m_pSampleModulConfigData->m_RMSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPCBServerIp:
            m_pSampleModulConfigData->m_PCBServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setPCBServerPort:
            m_pSampleModulConfigData->m_PCBServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setDSPServerIp:
            m_pSampleModulConfigData->m_DSPServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setDSPServerPort:
            m_pSampleModulConfigData->m_DSPServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSampleSystem:
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_sSampleSystem = m_pXMLReader->getValue(key);
            break;
        case setPllChannelCount:
        {
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_npllChannelCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for pll channel configuration
            QString channel;
            for (int i = 0; i < m_pSampleModulConfigData->m_ObsermaticConfPar.m_npllChannelCount; i++)
            {
                m_ConfigXMLMap[QString("samplemodconfpar:configuration:sample:pllchannel:ch%1").arg(i+1)] = setPllChannel1+i;
                m_pSampleModulConfigData->m_ObsermaticConfPar.m_pllChannelList.append(channel);
            }
            break;
        }
        case setPllAutomatic:
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_npllAutoAct.m_sKey = key;
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_npllAutoAct.m_nActive = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPllAutomaticBool:
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_bpllAuto = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setPllChannel:
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_pllChannel.m_sKey = key;
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_pllChannel.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setSampleSystemRange:
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_pllRange.m_sKey = key;
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_pllRange.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setMeasureInterval:
            m_pSampleModulConfigData->m_fMeasInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        default:
            if ((cmd >= setPllChannel1) && (cmd < setPllChannel1 + 32))
            {
                cmd -= setPllChannel1;
                m_pSampleModulConfigData->m_ObsermaticConfPar.m_pllChannelList.replace(cmd, m_pXMLReader->getValue(key));
            }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cSampleModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

}

