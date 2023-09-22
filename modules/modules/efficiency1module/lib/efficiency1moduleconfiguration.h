#ifndef EFFICIENCY1MODULECONFIGURATION_H
#define EFFICIENCY1MODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace EFFICIENCY1MODULE
{

enum moduleconfigstate
{
    setDebugLevel,

    setInputModule4InputPower,
    setInputSystemCount,

    setInputModule4OutputPower,
    setOutputSystemCount,

    setInputPowerMeasSystem1 = 16, // we leave some place for additional cmds

    setOutputPowerMeassystem1 = 32,

    setnext = setOutputPowerMeassystem1 + 16
};



class cEfficiency1ModuleConfigData;

const QString defaultXSDFile = "://efficiency1module.xsd";

// moduleconfiguration holds configuration data as well as parameter

class cEfficiency1ModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cEfficiency1ModuleConfiguration();
    ~cEfficiency1ModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cEfficiency1ModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cEfficiency1ModuleConfigData *m_pEfficiency1ModulConfigData;  // configuration
};

}

#endif // EFFICIENCY1MODULCONFIGURATION_H
