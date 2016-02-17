#ifndef SEC1MODULECONFIGURATION_H
#define SEC1MODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace SEC1MODULE
{

enum moduleconfigstate
{
    setDebugLevel,
    setRMIp,
    setRMPort,
    setPCBServerIp,
    setPCBServerPort,
    setSECServerIp,
    setSECServerPort,
    setMeasureInterval,
    setDutInputCount,
    setRefInputCount,
    setModeCount,
    setEmbedded,

    setDutInputPar,
    setRefInputPar,
    setMeasMode,
    setDutConstant,
    setRefConstant,
    setTarget,
    setEnergy,
    setMRate,

    setDutInput1Name = 32,
    setRefInput1Name = 64,
    setMode1Name = 96
};



class cSec1ModuleConfigData;

const QString defaultXSDFile = "://src/sec1module.xsd";

// sec1moduleconfiguration holds configuration data as well as parameter

class cSec1ModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cSec1ModuleConfiguration();
    ~cSec1ModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cSec1ModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    // QHash<QString, VeinEntity*> m_exportEntityList; // we hold a list of all our entities for export
    cSec1ModuleConfigData *m_pSec1ModulConfigData;  // configuration
};

}

#endif // SEC1MODULCONFIGURATION_H
