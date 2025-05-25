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
    setMeasModeCount,
    setMeasSystem1,
    setMeasSystem2,
    setMeasSystem3,
    setDisablePhaseSelect,
    setIntegrationMode,
    setMovingwindowBool,
    setMovingwindowTime,
    setNominalFrequency,
    setFrequencyActualizationMode,
    setFrequencyOutputCount,
    setEnableScpiCommands,

    setMeasuringMode,
    setModePhaseCount,
    setMeasureIntervalTime,
    setMeasureIntervalPeriod,
    setQREFFrequency,

    setMeasMode1 = 32, // we leave some place for additional cmds
    setMeasModePhases1 = setMeasMode1 + 32,

    setfreqout1Name = setMeasModePhases1 + 32, // 32 measuring modes should be enough
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

    setfreqout1NameDisplayed = setfreqout1Type + 8,
    setfreqout2Plug,
    setfreqout3Plug,
    setfreqout4Plug,

    setnext = setfreqout1NameDisplayed + 8,

    setfreqout1UScaleEntity = setnext+8,
    setfreqout2UScaleEntity,
    setfreqout3UScaleEntity,
    setfreqout4UScaleEntity,

    setfreqout1UScaleComponent = setfreqout1UScaleEntity + 8,
    setfreqout2UScaleComponent,
    setfreqout3UScaleComponent,
    setfreqout4UScaleComponent,

    setfreqout1IScaleEntity = setfreqout1UScaleComponent+8,
    setfreqout2IScaleEntity,
    setfreqout3IScaleEntity,
    setfreqout4IScaleEntity,

    setfreqout1IScaleComponent = setfreqout1IScaleEntity + 8,
    setfreqout2IScaleComponent,
    setfreqout3IScaleComponent,
    setfreqout4IScaleComponent
};

class cPower1ModuleConfigData;

// moduleconfiguration holds configuration data as well as parameter
class cPower1ModuleConfiguration: public BaseModuleConfiguration
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
    void addMeasSys(QString val);
    cPower1ModuleConfigData *m_pPower1ModulConfigData = nullptr;  // configuration
};

}

#endif // POWER1MODULCONFIGURATION_H
