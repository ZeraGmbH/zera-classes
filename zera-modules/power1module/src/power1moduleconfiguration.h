#ifndef POWER1MODULECONFIGURATION_H
#define POWER1MODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace POWER1MODULE
{

enum moduleconfigstate
{
    setDebugLevel,
    setRMIp,
    setRMPort,
    setPCBServerIp,
    setPCBServerPort,
    setDSPServerIp,
    setDSPServerPort,
    setMeasModeCount,
    setMeasSystem1,
    setMeasSystem2,
    setMeasSystem3,
    set2WMeasSystem,
    setIntegrationMode,
    setMovingwindowBool,
    setMovingwindowTime,
    setNominalFrequency,
    setFrequencyActualizationMode,
    setFrequencyOutputCount,

    setMeasuringMode,
    setMeasureIntervalTime,
    setMeasureIntervalPeriod,

    setMeasMode1 = 32, // we leave some place for additional cmds

    setfreqout1Name = setMeasMode1 + 32, // 32 measuring modes should be enough
    setfreqout2Name,
    setfreqout3Name,
    setfreqout4Name,

    setfreqout1Source = setfreqout1Name +8, // also some place for additional frequency outputs
    setfreqout2Source,
    setfreqout3Source,
    setfreqout4Source,

    setfreqout1Type = setfreqout1Source + 8,
    setfreqout2Type,
    setfreqout3Type,
    setfreqout4Type,

    setnext = setfreqout1Type + 8
};



class cPower1ModuleConfigData;

const QString defaultXSDFile = "://src/power1module.xsd";

// moduleconfiguration holds configuration data as well as parameter

class cPower1ModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cPower1ModuleConfiguration();
    ~cPower1ModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cPower1ModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cPower1ModuleConfigData *m_pPower1ModulConfigData;  // configuration
};

}

#endif // POWER1MODULCONFIGURATION_H
