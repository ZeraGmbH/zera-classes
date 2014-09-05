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
    setNominalFrequency,
    setFrequencyActualizationMode,
    setFrequencyOutputCount,

    setMeasureInterval,

    setMeasMode1 = 32,

    setfreqout1Name = setMeasMode1 + 32,
    setfreqout1Source,
    setfreqout1Type,

    setfreqout2Name = setfreqout1Name + 4,
    setfreqout2Source,
    setfreqout2Type,

    setfreqout3Name = setfreqout2Name + 4,
    setfreqout3Source,
    setfreqout3Type,

    setfreqout4Name = setfreqout3Name + 4,
    setfreqout4Source,
    setfreqout4Type,



    setnext = setfreqout4Name + 4
};



class cPower1ModuleConfigData;

//const QString defaultXSDFile = "/etc/zera/modules/power1module.xsd";
const QString defaultXSDFile = "/home/peter/C++/zera-classes/zera-modules/power1module/src/power1module.xsd";

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
