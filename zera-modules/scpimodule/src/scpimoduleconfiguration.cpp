#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "scpimoduleconfiguration.h"
#include "scpimoduleconfigdata.h"
#include "socket.h"

namespace SCPIMODULE
{

cSCPIModuleConfiguration::cSCPIModuleConfiguration()
{
    m_pSCPIModulConfigData = 0;
    connect(m_pXMLReader, SIGNAL(valueChanged(const QString&)), this, SLOT(configXMLInfo(const QString&)));
    connect(m_pXMLReader, SIGNAL(finishedParsingXML(bool)), this, SLOT(completeConfiguration(bool)));
}


cSCPIModuleConfiguration::~cSCPIModuleConfiguration()
{
    if (m_pSCPIModulConfigData) delete m_pSCPIModulConfigData;
}


void cSCPIModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pSCPIModulConfigData) delete m_pSCPIModulConfigData;
    m_pSCPIModulConfigData = new cSCPIModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:debuglevel"] = setDebugLevel;
    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:ethernet:nrclient"] = setnrClient;
    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:ethernet:port"] = setInterfacePort;
    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:device"] = setDeviceName;

    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:status:questionable:n"] = setQuestionableStatusBitCount;
    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:status:operation:n"] = setOperationStatusBitCount;
    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:status:operationmeasure:n"] = setOperationMeasureStatusBitCount;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cSCPIModuleConfiguration::exportConfiguration()
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}


cSCPIModuleConfigData *cSCPIModuleConfiguration::getConfigurationData()
{
    return m_pSCPIModulConfigData;
}


void cSCPIModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDebugLevel:
            m_pSCPIModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setnrClient:
            m_pSCPIModulConfigData->m_nClients = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setInterfacePort:
            m_pSCPIModulConfigData->m_InterfaceSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            m_pSCPIModulConfigData->m_InterfaceSocket.m_sIP = "127.0.0.1";
            break;
        case setDeviceName:
            m_pSCPIModulConfigData->m_sDeviceName = m_pXMLReader->getValue(key);
            break;

        case setQuestionableStatusBitCount:
            m_pSCPIModulConfigData->m_nQuestonionableStatusBitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSCPIModulConfigData->m_nQuestonionableStatusBitCount; i++)
            {
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:questionable:bit%1:bit").arg(i+1)] = setQuestionableBit1Bit+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:questionable:bit%1:module").arg(i+1)] = setQuestionableBit1Module+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:questionable:bit%1:entity").arg(i+1)] = setQuestionableBit1Entity+i;
                 // todo add something to some list
            }
            break;
        case setOperationStatusBitCount:
            m_pSCPIModulConfigData->m_nOperationStatusBitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSCPIModulConfigData->m_nOperationStatusBitCount; i++)
            {
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operation:bit%1:bit").arg(i+1)] = setOperationBit1Bit+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operation:bit%1:module").arg(i+1)] = setOperationBit1Module+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operation:bit%1:entity").arg(i+1)] = setOperationBit1Entity+i;
                 // todo add something to some list
            }
            break;
        case setOperationMeasureStatusBitCount:
            m_pSCPIModulConfigData->m_nOperationMeasureStatusBitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSCPIModulConfigData->m_nOperationMeasureStatusBitCount; i++)
            {
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operationmeasure:bit%1:bit").arg(i+1)] = setOperationMeasureBit1Bit+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operationmeasure:bit%1:module").arg(i+1)] = setOperationMeasureBit1Module+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operationmeasure:bit%1:entity").arg(i+1)] = setOperationMeasureBit1Entity+i;
                 // todo add something to some list
            }
            break;
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cSCPIModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

}

