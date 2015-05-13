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
    setDebugLevel,
    setnrClient,
    setInterfacePort,
    setDeviceName,

    setQuestionableStatusBitCount,
    setOperationStatusBitCount,
    setOperationMeasureStatusBitCount,

    setQuestionableBit1Bit = 16, // max. 16 bits
    setQuestionableBit1Module = 32, // max. 16 bits
    setQuestionableBit1Entity = 48, // max. 16 bits

    setOperationBit1Bit = 64, // max. 16 bits
    setOperationBit1Module = 96, // max. 16 bits
    setOperationBit1Entity = 128, // max. 16 bits

    setOperationMeasureBit1Bit = 160, // max. 16 bits
    setOperationMeasureBit1Module = 160, // max. 16 bits
    setOperationMeasureBit1Entity = 192, // max. 16 bits
};



class cSCPIModuleConfigData;

const QString defaultXSDFile = "://src/scpimodule.xsd";

// scpimoduleconfiguration holds configuration data

class cSCPIModuleConfiguration: public cBaseModuleConfiguration
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
    // QHash<QString, VeinEntity*> m_exportEntityList; // we hold a list of all our entities for export
    cSCPIModuleConfigData *m_pSCPIModulConfigData;  // configuration
};

}

#endif // SCPIMODULCONFIGURATION_H
