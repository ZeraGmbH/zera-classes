#ifndef POWER2MODULECONFIGURATION_H
#define POWER2MODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace POWER2MODULE
{

enum moduleconfigstate
{
    setMeasModeCount,
    setMeasSystem1,
    setMeasSystem2,
    setMeasSystem3,
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

class cPower2ModuleConfigData;

// moduleconfiguration holds configuration data as well as parameter
class cPower2ModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cPower2ModuleConfiguration();
    ~cPower2ModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cPower2ModuleConfigData* getConfigurationData();
protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);
private:
    void addMeasSys(QString val);
    cPower2ModuleConfigData *m_pPower2ModulConfigData = nullptr;  // configuration
};

}

#endif // POWER2MODULCONFIGURATION_H
