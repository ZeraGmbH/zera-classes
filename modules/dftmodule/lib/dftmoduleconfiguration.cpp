#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "dftmoduleconfiguration.h"
#include "dftmoduleconfigdata.h"
#include "networkconnectioninfo.h"

namespace DFTMODULE
{
static const char* defaultXSDFile = "://dftmodule.xsd";


cDftModuleConfiguration::cDftModuleConfiguration()
{
    m_pDftModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cDftModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cDftModuleConfiguration::completeConfiguration);
}


cDftModuleConfiguration::~cDftModuleConfiguration()
{
    if (m_pDftModulConfigData) delete m_pDftModulConfigData;
}


void cDftModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pDftModulConfigData) delete m_pDftModulConfigData;
    m_pDftModulConfigData = new cDftModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["dftmodconfpar:configuration:connectivity:ethernet:resourcemanager:ip"] = setRMIp;
    m_ConfigXMLMap["dftmodconfpar:configuration:connectivity:ethernet:resourcemanager:port"] = setRMPort;
    m_ConfigXMLMap["dftmodconfpar:configuration:connectivity:ethernet:pcbserver:ip"] = setPCBServerIp;
    m_ConfigXMLMap["dftmodconfpar:configuration:connectivity:ethernet:pcbserver:port"] = setPCBServerPort;
    m_ConfigXMLMap["dftmodconfpar:configuration:connectivity:ethernet:dspserver:ip"] = setDSPServerIp;
    m_ConfigXMLMap["dftmodconfpar:configuration:connectivity:ethernet:dspserver:port"] = setDSPServerPort;
    m_ConfigXMLMap["dftmodconfpar:configuration:measure:dftvalues:nr"] = setDftOrder;
    m_ConfigXMLMap["dftmodconfpar:configuration:measure:dftvalues:n"] = setValueCount;
    m_ConfigXMLMap["dftmodconfpar:configuration:measure:rfield:val1"] = setrfieldChn1;
    m_ConfigXMLMap["dftmodconfpar:configuration:measure:rfield:val2"] = setrfieldChn2;
    m_ConfigXMLMap["dftmodconfpar:configuration:measure:rfield:val3"] = setrfieldChn3;
    m_ConfigXMLMap["dftmodconfpar:configuration:measure:movingwindow:on"] = setMovingwindowBool;
    m_ConfigXMLMap["dftmodconfpar:configuration:measure:movingwindow:time"] = setMovingwindowTime;
    m_ConfigXMLMap["dftmodconfpar:configuration:measure:refchannelon"] = setRefChannelOn;

    m_ConfigXMLMap["dftmodconfpar:parameter:interval"] = setMeasureInterval;
    m_ConfigXMLMap["dftmodconfpar:parameter:refchannel"] = setRefChannel;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cDftModuleConfiguration::exportConfiguration()
{
    doubleParameter* dPar;
    dPar = &m_pDftModulConfigData->m_fMeasInterval;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fValue));

    stringParameter* sPar;
    sPar = &m_pDftModulConfigData->m_sRefChannel;
    m_pXMLReader->setValue(sPar->m_sKey, sPar->m_sPar);

    return m_pXMLReader->getXMLConfig().toUtf8();
}


cDftModuleConfigData *cDftModuleConfiguration::getConfigurationData()
{
    return m_pDftModulConfigData;
}


void cDftModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setRMIp:
            m_pDftModulConfigData->m_RMSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setRMPort:
            m_pDftModulConfigData->m_RMSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPCBServerIp:
            m_pDftModulConfigData->m_PCBServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setPCBServerPort:
            m_pDftModulConfigData->m_PCBServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setDSPServerIp:
            m_pDftModulConfigData->m_DSPServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setDSPServerPort:
            m_pDftModulConfigData->m_DSPServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setDftOrder:
            m_pDftModulConfigData->m_nDftOrder = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setValueCount:
            m_pDftModulConfigData->m_nValueCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_pDftModulConfigData->m_nValueCount; i++)
                m_ConfigXMLMap[QString("dftmodconfpar:configuration:measure:dftvalues:val%1").arg(i+1)] = setValue1+i;
            break;
        case setrfieldChn1:
        case setrfieldChn2:
        case setrfieldChn3:
            m_pDftModulConfigData->m_rfieldChannelList.append(m_pXMLReader->getValue(key));
            break;
        case setMovingwindowBool:
            m_pDftModulConfigData->m_bmovingWindow = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setMovingwindowTime:
            m_pDftModulConfigData->m_fmovingwindowInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setMeasureInterval:
            m_pDftModulConfigData->m_fMeasInterval.m_sKey = key;
            m_pDftModulConfigData->m_fMeasInterval.m_fValue = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setRefChannel:
            m_pDftModulConfigData->m_sRefChannel.m_sKey = key;
            m_pDftModulConfigData->m_sRefChannel.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setRefChannelOn:
            m_pDftModulConfigData->m_bRefChannelOn = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;

        default:
            if ((cmd >= setValue1) && (cmd < setValue1 + 32))
            {
                cmd -= setValue1;
                // it is command for setting value channel name
                QString valueChannel = m_pXMLReader->getValue(key);
                m_pDftModulConfigData->m_valueChannelList.append(valueChannel); // for configuration of our engine
            }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cDftModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

}

