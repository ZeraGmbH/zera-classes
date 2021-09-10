#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "modemoduleconfiguration.h"
#include "modemoduleconfigdata.h"
#include "socket.h"

namespace MODEMODULE
{

cModeModuleConfiguration::cModeModuleConfiguration()
{
    m_pModeModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cModeModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cModeModuleConfiguration::completeConfiguration);
}


cModeModuleConfiguration::~cModeModuleConfiguration()
{
    if (m_pModeModulConfigData) delete m_pModeModulConfigData;
}


void cModeModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pModeModulConfigData) delete m_pModeModulConfigData;
    m_pModeModulConfigData = new cModeModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["modemodconfpar:configuration:connectivity:debuglevel"] = setDebugLevel;
    m_ConfigXMLMap["modemodconfpar:configuration:connectivity:ethernet:resourcemanager:ip"] = setRMIp;
    m_ConfigXMLMap["modemodconfpar:configuration:connectivity:ethernet:resourcemanager:port"] = setRMPort;
    m_ConfigXMLMap["modemodconfpar:configuration:connectivity:ethernet:pcbserver:ip"] = setPCBServerIp;
    m_ConfigXMLMap["modemodconfpar:configuration:connectivity:ethernet:pcbserver:port"] = setPCBServerPort;
    m_ConfigXMLMap["modemodconfpar:configuration:connectivity:ethernet:dspserver:ip"] = setDSPServerIp;
    m_ConfigXMLMap["modemodconfpar:configuration:connectivity:ethernet:dspserver:port"] = setDSPServerPort;

    m_ConfigXMLMap["modemodconfpar:configuration:pcb:mode"] = setMeasMode;
    m_ConfigXMLMap["modemodconfpar:configuration:dsp:sampling:chnnr"] = setSamplingChnNr;
    m_ConfigXMLMap["modemodconfpar:configuration:dsp:sampling:signalperiod"] = setSamplingSigPeriod;
    m_ConfigXMLMap["modemodconfpar:configuration:dsp:sampling:measureperiod"] = setSamplingMeasPeriod;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cModeModuleConfiguration::exportConfiguration()
{
    // nothing to set befor export
    return m_pXMLReader->getXMLConfig().toUtf8();
}


cModeModuleConfigData *cModeModuleConfiguration::getConfigurationData()
{
    return m_pModeModulConfigData;
}


void cModeModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDebugLevel:
            m_pModeModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setRMIp:
            m_pModeModulConfigData->m_RMSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setRMPort:
            m_pModeModulConfigData->m_RMSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPCBServerIp:
            m_pModeModulConfigData->m_PCBServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setPCBServerPort:
            m_pModeModulConfigData->m_PCBServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setDSPServerIp:
            m_pModeModulConfigData->m_DSPServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setDSPServerPort:
            m_pModeModulConfigData->m_DSPServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setMeasMode:
            m_pModeModulConfigData->m_sMode = m_pXMLReader->getValue(key);
            break;
        case setSamplingChnNr:
            m_pModeModulConfigData->m_nChannelnr = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSamplingSigPeriod:
            m_pModeModulConfigData->m_nSignalPeriod = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSamplingMeasPeriod:
            m_pModeModulConfigData->m_nMeasurePeriod = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        }

        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cModeModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

}

