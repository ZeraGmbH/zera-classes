#include "scpimoduleconfiguration.h"
#include "scpimoduleconfigdata.h"
#include <xmlconfigreader.h>

static void initResource()
{
    Q_INIT_RESOURCE(scpimodulexmlschema);
}

namespace SCPIMODULE
{
static const char *defaultXSDFile = "://scpimodule.xsd";

cSCPIModuleConfiguration::cSCPIModuleConfiguration()
{
    initResource();
    m_pSCPIModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cSCPIModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cSCPIModuleConfiguration::completeConfiguration);
}

cSCPIModuleConfiguration::~cSCPIModuleConfiguration()
{
    if (m_pSCPIModulConfigData) delete m_pSCPIModulConfigData;
}

void cSCPIModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    validateAndSetConfig(xmlString, defaultXSDFile);
}

void cSCPIModuleConfiguration::validateAndSetConfig(QByteArray xmlString, QString xsdFilename)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pSCPIModulConfigData) delete m_pSCPIModulConfigData;
    m_pSCPIModulConfigData = new cSCPIModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:ethernet:nrclient"] = setnrClient;
    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:ethernet:port"] = setInterfacePort;
    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:serialdevice:on"] = setSerialOn;
    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:serialdevice:baud"] = setSerialBaud;
    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:serialdevice:databits"] = setSerialDatabits;
    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:serialdevice:stopbits"] = setSerialStopbits;
    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:serialdevice:device"] = setSerialDevice;

    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:device:name"] = setDeviceName;

    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:status:questionable:n"] = setQuestionableStatusBitCount;
    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:status:operation:n"] = setOperationStatusBitCount;
    m_ConfigXMLMap["scpimodconfpar:configuration:connectivity:status:operationmeasure:n"] = setOperationMeasureStatusBitCount;
    m_ConfigXMLMap["scpimodconfpar:configuration:scpiqueue"] = setScpiQueue;

    if (m_pXMLReader->loadSchema(xsdFilename))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cSCPIModuleConfiguration::exportConfiguration()
{
    boolParameter* bPar;
    bPar = &m_pSCPIModulConfigData->m_enableScpiQueue;
    m_pXMLReader->setValue(bPar->m_sKey, QString("%1").arg(bPar->m_nActive));

    m_pXMLReader->setValue("scpimodconfpar:configuration:connectivity:serialdevice:on",
                           QString("%1").arg(m_pSCPIModulConfigData->m_SerialDevice.m_nOn));

    return m_pXMLReader->getXMLConfig().toUtf8();
}


cSCPIModuleConfigData *cSCPIModuleConfiguration::getConfigurationData()
{
    return m_pSCPIModulConfigData;
}

void cSCPIModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;
    cStatusBitDescriptor sBDescriptor;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setnrClient:
            m_pSCPIModulConfigData->m_nClients = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setInterfacePort:
            m_pSCPIModulConfigData->m_InterfaceSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            m_pSCPIModulConfigData->m_InterfaceSocket.m_sIP = "127.0.0.1";
            break;
        case setSerialOn:
            m_pSCPIModulConfigData->m_SerialDevice.m_nOn = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSerialBaud:
            m_pSCPIModulConfigData->m_SerialDevice.m_nBaud = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSerialDatabits:
            m_pSCPIModulConfigData->m_SerialDevice.m_nDatabits = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSerialStopbits:
            m_pSCPIModulConfigData->m_SerialDevice.m_nStopbits = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSerialDevice:
            m_pSCPIModulConfigData->m_SerialDevice.m_sDevice = m_pXMLReader->getValue(key);
            break;
        case setDeviceName:
            m_pSCPIModulConfigData->m_sDeviceName = m_pXMLReader->getValue(key);
            break;
        case setScpiQueue:
            m_pSCPIModulConfigData->m_enableScpiQueue.m_sKey = key;
            m_pSCPIModulConfigData->m_enableScpiQueue.m_nActive = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setQuestionableStatusBitCount:
            m_pSCPIModulConfigData->m_nQuestonionableStatusBitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSCPIModulConfigData->m_nQuestonionableStatusBitCount; i++)
            {
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:questionable:bit%1:bit").arg(i+1)] = setQuestionableBit1Bit+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:questionable:bit%1:module").arg(i+1)] = setQuestionableBit1Module+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:questionable:bit%1:component").arg(i+1)] = setQuestionableBit1Component+i;
                 m_pSCPIModulConfigData->m_QuestionableStatDescriptorList.append(sBDescriptor);  // we add empty descriptors to our list
            }
            break;
        case setOperationStatusBitCount:
            m_pSCPIModulConfigData->m_nOperationStatusBitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSCPIModulConfigData->m_nOperationStatusBitCount; i++)
            {
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operation:bit%1:bit").arg(i+1)] = setOperationBit1Bit+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operation:bit%1:module").arg(i+1)] = setOperationBit1Module+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operation:bit%1:component").arg(i+1)] = setOperationBit1Component+i;
                 m_pSCPIModulConfigData->m_OperationStatDescriptorList.append(sBDescriptor);  // we add empty descriptors to our list
            }
            break;
        case setOperationMeasureStatusBitCount:
            m_pSCPIModulConfigData->m_nOperationMeasureStatusBitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSCPIModulConfigData->m_nOperationMeasureStatusBitCount; i++)
            {
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operationmeasure:bit%1:bit").arg(i+1)] = setOperationMeasureBit1Bit+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operationmeasure:bit%1:module").arg(i+1)] = setOperationMeasureBit1Module+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operationmeasure:bit%1:component").arg(i+1)] = setOperationMeasureBit1Component+i;
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
                    sBDescriptor.m_sSCPIModuleName = m_pXMLReader->getValue(key);
                    m_pSCPIModulConfigData->m_QuestionableStatDescriptorList.replace(cmd, sBDescriptor);
                }
                else
                    if ((cmd >=setQuestionableBit1Component) && (cmd <= (setQuestionableBit1Component+15)))
                    {
                        cmd -= setQuestionableBit1Component;
                        sBDescriptor = m_pSCPIModulConfigData->m_QuestionableStatDescriptorList.at(cmd);
                        sBDescriptor.m_sComponentName = m_pXMLReader->getValue(key);
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
                                sBDescriptor.m_sSCPIModuleName = m_pXMLReader->getValue(key);
                                m_pSCPIModulConfigData->m_OperationStatDescriptorList.replace(cmd, sBDescriptor);
                            }
                            else
                                if ((cmd >=setOperationBit1Component) && (cmd <= (setOperationBit1Component+15)))
                                {
                                    cmd -= setOperationBit1Component;
                                    sBDescriptor = m_pSCPIModulConfigData->m_OperationStatDescriptorList.at(cmd);
                                    sBDescriptor.m_sComponentName = m_pXMLReader->getValue(key);
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
                                            sBDescriptor.m_sSCPIModuleName = m_pXMLReader->getValue(key);
                                            m_pSCPIModulConfigData->m_OperationMeasureStatDescriptorList.replace(cmd, sBDescriptor);
                                        }
                                        else
                                            if ((cmd >=setOperationMeasureBit1Component) && (cmd <= (setOperationMeasureBit1Component+15)))
                                            {
                                                cmd -= setOperationMeasureBit1Component;
                                                sBDescriptor = m_pSCPIModulConfigData->m_OperationMeasureStatDescriptorList.at(cmd);
                                                sBDescriptor.m_sComponentName = m_pXMLReader->getValue(key);
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
}

}

