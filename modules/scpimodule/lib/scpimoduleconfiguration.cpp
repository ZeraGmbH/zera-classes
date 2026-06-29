#include "scpimoduleconfiguration.h"

namespace SCPIMODULE
{

cSCPIModuleConfiguration::cSCPIModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setnrClient,
    setInterfacePort,
    setSerialOn,
    setSerialBaud,
    setSerialDatabits,
    setSerialStopbits,
    setSerialDevice,
    setDeviceName,

    setQuestionableStatusBitCount,
    setOperationStatusBitCount,
    setOperationMeasureStatusBitCount,

    setQuestionableBit1Bit = 16, // max. 16 bits
    setQuestionableBit1Module = 32, // max. 16 bits
    setQuestionableBit1Component = 48, // max. 16 bits

    setOperationBit1Bit = 64, // max. 16 bits
    setOperationBit1Module = 80, // max. 16 bits
    setOperationBit1Component = 96, // max. 16 bits

    setOperationMeasureBit1Bit = 112, // max. 16 bits
    setOperationMeasureBit1Module = 128, // max. 16 bits
    setOperationMeasureBit1Component = 144, // max. 16 bits
};

void cSCPIModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
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

    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cSCPIModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cSCPIModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}


QByteArray cSCPIModuleConfiguration::exportConfiguration() const
{
    m_pXMLReader->setValue("scpimodconfpar:configuration:connectivity:serialdevice:on",
                           QString("%1").arg(m_configData.m_SerialDevice.m_nOn));

    return m_pXMLReader->getXMLConfig().toUtf8();
}


cSCPIModuleConfigData *cSCPIModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cSCPIModuleConfiguration::configXMLInfo(const QString &key)
{

    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        cStatusBitDescriptor sBDescriptor;
        switch (cmd)
        {
        case setnrClient:
            m_configData.m_nClients = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setInterfacePort:
            m_configData.m_InterfaceSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            m_configData.m_InterfaceSocket.m_sIP = "127.0.0.1";
            break;
        case setSerialOn:
            m_configData.m_SerialDevice.m_nOn = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSerialBaud:
            m_configData.m_SerialDevice.m_nBaud = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSerialDatabits:
            m_configData.m_SerialDevice.m_nDatabits = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSerialStopbits:
            m_configData.m_SerialDevice.m_nStopbits = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSerialDevice:
            m_configData.m_SerialDevice.m_sDevice = m_pXMLReader->getValue(key);
            break;
        case setDeviceName:
            m_configData.m_sDeviceName = m_pXMLReader->getValue(key);
            break;
        case setQuestionableStatusBitCount:
            m_configData.m_nQuestonionableStatusBitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_configData.m_nQuestonionableStatusBitCount; i++)
            {
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:questionable:bit%1:bit").arg(i+1)] = setQuestionableBit1Bit+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:questionable:bit%1:module").arg(i+1)] = setQuestionableBit1Module+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:questionable:bit%1:component").arg(i+1)] = setQuestionableBit1Component+i;
                 m_configData.m_QuestionableStatDescriptorList.append(sBDescriptor);  // we add empty descriptors to our list
            }
            break;
        case setOperationStatusBitCount:
            m_configData.m_nOperationStatusBitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_configData.m_nOperationStatusBitCount; i++)
            {
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operation:bit%1:bit").arg(i+1)] = setOperationBit1Bit+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operation:bit%1:module").arg(i+1)] = setOperationBit1Module+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operation:bit%1:component").arg(i+1)] = setOperationBit1Component+i;
                 m_configData.m_OperationStatDescriptorList.append(sBDescriptor);  // we add empty descriptors to our list
            }
            break;
        case setOperationMeasureStatusBitCount:
            m_configData.m_nOperationMeasureStatusBitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_configData.m_nOperationMeasureStatusBitCount; i++)
            {
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operationmeasure:bit%1:bit").arg(i+1)] = setOperationMeasureBit1Bit+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operationmeasure:bit%1:module").arg(i+1)] = setOperationMeasureBit1Module+i;
                 m_ConfigXMLMap[QString("scpimodconfpar:configuration:connectivity:status:operationmeasure:bit%1:component").arg(i+1)] = setOperationMeasureBit1Component+i;
                 m_configData.m_OperationMeasureStatDescriptorList.append(sBDescriptor);  // we add empty descriptors to our list
            }
            break;
        default:
            if ((cmd >=setQuestionableBit1Bit) && (cmd <= (setQuestionableBit1Bit+15)))
            {
                cmd -= setQuestionableBit1Bit;
                sBDescriptor = m_configData.m_QuestionableStatDescriptorList.at(cmd);
                sBDescriptor.m_nBitNr = m_pXMLReader->getValue(key).toInt(&ok);
                m_configData.m_QuestionableStatDescriptorList.replace(cmd, sBDescriptor);
            }
            else
                if ((cmd >=setQuestionableBit1Module) && (cmd <= (setQuestionableBit1Module+15)))
                {
                    cmd -= setQuestionableBit1Module;
                    sBDescriptor = m_configData.m_QuestionableStatDescriptorList.at(cmd);
                    sBDescriptor.m_sSCPIModuleName = m_pXMLReader->getValue(key);
                    m_configData.m_QuestionableStatDescriptorList.replace(cmd, sBDescriptor);
                }
                else
                    if ((cmd >=setQuestionableBit1Component) && (cmd <= (setQuestionableBit1Component+15)))
                    {
                        cmd -= setQuestionableBit1Component;
                        sBDescriptor = m_configData.m_QuestionableStatDescriptorList.at(cmd);
                        sBDescriptor.m_sComponentName = m_pXMLReader->getValue(key);
                        m_configData.m_QuestionableStatDescriptorList.replace(cmd, sBDescriptor);
                    }

                    else

                        if ((cmd >=setOperationBit1Bit) && (cmd <= (setOperationBit1Bit+15)))
                        {
                            cmd -= setOperationBit1Bit;
                            sBDescriptor = m_configData.m_OperationStatDescriptorList.at(cmd);
                            sBDescriptor.m_nBitNr = m_pXMLReader->getValue(key).toInt(&ok);
                            m_configData.m_OperationStatDescriptorList.replace(cmd, sBDescriptor);
                        }
                        else
                            if ((cmd >=setOperationBit1Module) && (cmd <= (setOperationBit1Module+15)))
                            {
                                cmd -= setOperationBit1Module;
                                sBDescriptor = m_configData.m_OperationStatDescriptorList.at(cmd);
                                sBDescriptor.m_sSCPIModuleName = m_pXMLReader->getValue(key);
                                m_configData.m_OperationStatDescriptorList.replace(cmd, sBDescriptor);
                            }
                            else
                                if ((cmd >=setOperationBit1Component) && (cmd <= (setOperationBit1Component+15)))
                                {
                                    cmd -= setOperationBit1Component;
                                    sBDescriptor = m_configData.m_OperationStatDescriptorList.at(cmd);
                                    sBDescriptor.m_sComponentName = m_pXMLReader->getValue(key);
                                    m_configData.m_OperationStatDescriptorList.replace(cmd, sBDescriptor);
                                }

                                else

                                    if ((cmd >=setOperationMeasureBit1Bit) && (cmd <= (setOperationMeasureBit1Bit+15)))
                                    {
                                        cmd -= setOperationMeasureBit1Bit;
                                        sBDescriptor = m_configData.m_OperationMeasureStatDescriptorList.at(cmd);
                                        sBDescriptor.m_nBitNr = m_pXMLReader->getValue(key).toInt(&ok);
                                        m_configData.m_OperationMeasureStatDescriptorList.replace(cmd, sBDescriptor);
                                    }
                                    else
                                        if ((cmd >=setOperationMeasureBit1Module) && (cmd <= (setOperationMeasureBit1Module+15)))
                                        {
                                            cmd -= setOperationMeasureBit1Module;
                                            sBDescriptor = m_configData.m_OperationMeasureStatDescriptorList.at(cmd);
                                            sBDescriptor.m_sSCPIModuleName = m_pXMLReader->getValue(key);
                                            m_configData.m_OperationMeasureStatDescriptorList.replace(cmd, sBDescriptor);
                                        }
                                        else
                                            if ((cmd >=setOperationMeasureBit1Component) && (cmd <= (setOperationMeasureBit1Component+15)))
                                            {
                                                cmd -= setOperationMeasureBit1Component;
                                                sBDescriptor = m_configData.m_OperationMeasureStatDescriptorList.at(cmd);
                                                sBDescriptor.m_sComponentName = m_pXMLReader->getValue(key);
                                                m_configData.m_OperationMeasureStatDescriptorList.replace(cmd, sBDescriptor);
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

