#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "fftmoduleconfiguration.h"
#include "fftmoduleconfigdata.h"
#include "networkconnectioninfo.h"

namespace FFTMODULE
{
static const char* defaultXSDFile = "://fftmodule.xsd";

cFftModuleConfiguration::cFftModuleConfiguration()
{
    m_pFftModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cFftModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cFftModuleConfiguration::completeConfiguration);
}


cFftModuleConfiguration::~cFftModuleConfiguration()
{
    if (m_pFftModulConfigData) delete m_pFftModulConfigData;
}


void cFftModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pFftModulConfigData) delete m_pFftModulConfigData;
    m_pFftModulConfigData = new cFftModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["fftmodconfpar:configuration:connectivity:ethernet:resourcemanager:ip"] = setRMIp;
    m_ConfigXMLMap["fftmodconfpar:configuration:connectivity:ethernet:resourcemanager:port"] = setRMPort;
    m_ConfigXMLMap["fftmodconfpar:configuration:connectivity:ethernet:pcbserver:ip"] = setPCBServerIp;
    m_ConfigXMLMap["fftmodconfpar:configuration:connectivity:ethernet:pcbserver:port"] = setPCBServerPort;
    m_ConfigXMLMap["fftmodconfpar:configuration:connectivity:ethernet:dspserver:ip"] = setDSPServerIp;
    m_ConfigXMLMap["fftmodconfpar:configuration:connectivity:ethernet:dspserver:port"] = setDSPServerPort;
    m_ConfigXMLMap["fftmodconfpar:configuration:measure:values:nr"] = setFftorder;
    m_ConfigXMLMap["fftmodconfpar:configuration:measure:values:n"] = setValueCount;
    m_ConfigXMLMap["fftmodconfpar:configuration:measure:movingwindow:on"] = setMovingwindowBool;
    m_ConfigXMLMap["fftmodconfpar:configuration:measure:movingwindow:time"] = setMovingwindowTime;

    m_ConfigXMLMap["fftmodconfpar:parameter:refchannel"] = setRefChannel;
    m_ConfigXMLMap["fftmodconfpar:parameter:interval"] = setMeasureInterval;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cFftModuleConfiguration::exportConfiguration()
{
    doubleParameter* dPar;
    dPar = &m_pFftModulConfigData->m_fMeasInterval;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fValue));
    return m_pXMLReader->getXMLConfig().toUtf8();
}


cFftModuleConfigData *cFftModuleConfiguration::getConfigurationData()
{
    return m_pFftModulConfigData;
}


void cFftModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setRMIp:
            m_pFftModulConfigData->m_RMSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setRMPort:
            m_pFftModulConfigData->m_RMSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPCBServerIp:
            m_pFftModulConfigData->m_PCBServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setPCBServerPort:
            m_pFftModulConfigData->m_PCBServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setDSPServerIp:
            m_pFftModulConfigData->m_DSPServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setDSPServerPort:
            m_pFftModulConfigData->m_DSPServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setFftorder:
            m_pFftModulConfigData->m_nFftOrder = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setValueCount:
            m_pFftModulConfigData->m_nValueCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_pFftModulConfigData->m_nValueCount; i++)
                m_ConfigXMLMap[QString("fftmodconfpar:configuration:measure:values:val%1").arg(i+1)] = setValue1+i;
            break;
        case setMovingwindowBool:
            m_pFftModulConfigData->m_bmovingWindow = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setMovingwindowTime:
            m_pFftModulConfigData->m_fmovingwindowInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setRefChannel:
            m_pFftModulConfigData->m_RefChannel.m_sKey = key;
            m_pFftModulConfigData->m_RefChannel.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setMeasureInterval:
            m_pFftModulConfigData->m_fMeasInterval.m_sKey = key;
            m_pFftModulConfigData->m_fMeasInterval.m_fValue = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        default:
            if ((cmd >= setValue1) && (cmd < setValue1 + 32))
            {
                cmd -= setValue1;
                // it is command for setting value channel name
                QString valueChannel = m_pXMLReader->getValue(key);
                m_pFftModulConfigData->m_valueChannelList.append(valueChannel); // for configuration of our engine
            }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cFftModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

}

