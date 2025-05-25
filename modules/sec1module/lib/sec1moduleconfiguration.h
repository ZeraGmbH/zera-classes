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
    setDutInputCount,
    setRefInputCount,

    setDutInputPar,
    setRefInputPar,
    setMeasMode,
    setDutConstant,
    setDutConstantUnit,
    setRefConstant,
    setTarget,
    setEnergy,
    setMRate,
    setContinousMode,
    setUpperLimit,
    setLowerLimit,
    setResultUnit,

    setDutInput1Name = 32,
    setRefInput1Name = 64,
    setRefInput1Append = 96
};



class cSec1ModuleConfigData;

// sec1moduleconfiguration holds configuration data as well as parameter

class cSec1ModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cSec1ModuleConfiguration();
    ~cSec1ModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString) override;
    virtual QByteArray exportConfiguration() override; // exports conf. and parameters to xml
    cSec1ModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key) override;
    virtual void completeConfiguration(bool ok);

private:
    // QHash<QString, VeinEntity*> m_exportEntityList; // we hold a list of all our entities for export
    cSec1ModuleConfigData *m_pSec1ModulConfigData = nullptr;  // configuration
};

}

#endif // SEC1MODULCONFIGURATION_H
