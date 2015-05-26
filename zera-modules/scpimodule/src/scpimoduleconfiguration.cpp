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
    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:device:name"] = setDeviceName;
    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:device:identification"] = setDeviceIdentification;

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
    statusBitDescriptor sBDescriptor;

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
        case setDeviceIdentification:
            m_pSCPIModulConfigData->m_sDeviceIdentification = m_pXMLReader->getValue(key);
            break;
        case setQuestionableStatusBitCount:
            m_pSCPIModulConfigData->m_nQuestonionableStatusBitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSCPIModulConfigData->m_nQuestonionableStatusBitCount; i++)
            {
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:questionable:bit%1:bit").arg(i+1)] = setQuestionableBit1Bit+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:questionable:bit%1:module").arg(i+1)] = setQuestionableBit1Module+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:questionable:bit%1:entity").arg(i+1)] = setQuestionableBit1Entity+i;
                 m_pSCPIModulConfigData->m_QuestionableStatDescriptorList.append(sBDescriptor);  // we add empty descriptors to our list
            }
            break;
        case setOperationStatusBitCount:
            m_pSCPIModulConfigData->m_nOperationStatusBitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSCPIModulConfigData->m_nOperationStatusBitCount; i++)
            {
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operation:bit%1:bit").arg(i+1)] = setOperationBit1Bit+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operation:bit%1:module").arg(i+1)] = setOperationBit1Module+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operation:bit%1:entity").arg(i+1)] = setOperationBit1Entity+i;
                 m_pSCPIModulConfigData->m_OperationStatDescriptorList.append(sBDescriptor);  // we add empty descriptors to our list
            }
            break;
        case setOperationMeasureStatusBitCount:
            m_pSCPIModulConfigData->m_nOperationMeasureStatusBitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSCPIModulConfigData->m_nOperationMeasureStatusBitCount; i++)
            {
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operationmeasure:bit%1:bit").arg(i+1)] = setOperationMeasureBit1Bit+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operationmeasure:bit%1:module").arg(i+1)] = setOperationMeasureBit1Module+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operationmeasure:bit%1:entity").arg(i+1)] = setOperationMeasureBit1Entity+i;
                 m_pSCPIModulConfigData->m_OperationMeasureStatDescriptorList.append(sBDescriptor);  // we add empty descriptors to our list
            }
            break;

        default:
            if ((cmd >=setQuestionableBit1Bit) && (cmd <= (setQuestionableBit1Bit+15)))
            {
                cmd -= setQuestionableBit1Bit;
                sBDescriptor = m_pSCPIModulConfigData->m_QuestionableStatDescriptorList.at(cmd);
                sBDescriptor.m_nBitNr = m_pXMLReader->getValue(key).toInt(&ok);
                m_pSCPIModulConfigData->m_QuestionableStatDescriptorList.replace(cmd, sBDescriptor);
            }
            else
                if ((cmd >=setQuestionableBit1Module) && (cmd <= (setQuestionableBit1Module+15)))
                {
                    cmd -= setQuestionableBit1Module;
                    sBDescriptor = m_pSCPIModulConfigData->m_QuestionableStatDescriptorList.at(cmd);
                    sBDescriptor.m_sModule = m_pXMLReader->getValue(key);
                    m_pSCPIModulConfigData->m_QuestionableStatDescriptorList.replace(cmd, sBDescriptor);
                }
                else
                    if ((cmd >=setQuestionableBit1Entity) && (cmd <= (setQuestionableBit1Entity+15)))
                    {
                        cmd -= setQuestionableBit1Entity;
                        sBDescriptor = m_pSCPIModulConfigData->m_QuestionableStatDescriptorList.at(cmd);
                        sBDescriptor.m_sEntity = m_pXMLReader->getValue(key);
                        m_pSCPIModulConfigData->m_QuestionableStatDescriptorList.replace(cmd, sBDescriptor);
                    }

                    else

                        if ((cmd >=setOperationBit1Bit) && (cmd <= (setOperationBit1Bit+15)))
                        {
                            cmd -= setOperationBit1Bit;
                            sBDescriptor = m_pSCPIModulConfigData->m_OperationStatDescriptorList.at(cmd);
                            sBDescriptor.m_nBitNr = m_pXMLReader->getValue(key).toInt(&ok);
                            m_pSCPIModulConfigData->m_OperationStatDescriptorList.replace(cmd, sBDescriptor);
                        }
                        else
                            if ((cmd >=setOperationBit1Module) && (cmd <= (setOperationBit1Module+15)))
                            {
                                cmd -= setOperationBit1Module;
                                sBDescriptor = m_pSCPIModulConfigData->m_OperationStatDescriptorList.at(cmd);
                                sBDescriptor.m_sModule = m_pXMLReader->getValue(key);
                                m_pSCPIModulConfigData->m_OperationStatDescriptorList.replace(cmd, sBDescriptor);
                            }
                            else
                                if ((cmd >=setOperationBit1Entity) && (cmd <= (setOperationBit1Entity+15)))
                                {
                                    cmd -= setOperationBit1Entity;
                                    sBDescriptor = m_pSCPIModulConfigData->m_OperationStatDescriptorList.at(cmd);
                                    sBDescriptor.m_sEntity = m_pXMLReader->getValue(key);
                                    m_pSCPIModulConfigData->m_OperationStatDescriptorList.replace(cmd, sBDescriptor);
                                }

                                else

                                    if ((cmd >=setOperationMeasureBit1Bit) && (cmd <= (setOperationMeasureBit1Bit+15)))
                                    {
                                        cmd -= setOperationMeasureBit1Bit;
                                        sBDescriptor = m_pSCPIModulConfigData->m_OperationMeasureStatDescriptorList.at(cmd);
                                        sBDescriptor.m_nBitNr = m_pXMLReader->getValue(key).toInt(&ok);
                                        m_pSCPIModulConfigData->m_OperationMeasureStatDescriptorList.replace(cmd, sBDescriptor);
                                    }
                                    else
                                        if ((cmd >=setOperationMeasureBit1Module) && (cmd <= (setOperationMeasureBit1Module+15)))
                                        {
                                            cmd -= setOperationMeasureBit1Module;
                                            sBDescriptor = m_pSCPIModulConfigData->m_OperationMeasureStatDescriptorList.at(cmd);
                                            sBDescriptor.m_sModule = m_pXMLReader->getValue(key);
                                            m_pSCPIModulConfigData->m_OperationMeasureStatDescriptorList.replace(cmd, sBDescriptor);
                                        }
                                        else
                                            if ((cmd >=setOperationMeasureBit1Entity) && (cmd <= (setOperationMeasureBit1Entity+15)))
                                            {
                                                cmd -= setOperationMeasureBit1Entity;
                                                sBDescriptor = m_pSCPIModulConfigData->m_OperationMeasureStatDescriptorList.at(cmd);
                                                sBDescriptor.m_sEntity = m_pXMLReader->getValue(key);
                                                m_pSCPIModulConfigData->m_OperationMeasureStatDescriptorList.replace(cmd, sBDescriptor);
                                            }


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

