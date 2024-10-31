#ifndef SCPIMODULECONFIGURATION_H
#define SCPIMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace SCPIMODULE
{

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
    setScpiQueue,

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

class cSCPIModuleConfigData;

// scpimoduleconfiguration holds configuration data
class cSCPIModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cSCPIModuleConfiguration();
    ~cSCPIModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cSCPIModuleConfigData* getConfigurationData();
protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);
private:
    void validateAndSetConfig(QByteArray xmlString, QString xsdFilename);
    cSCPIModuleConfigData *m_pSCPIModulConfigData;  // configuration
};

}

#endif // SCPIMODULCONFIGURATION_H
