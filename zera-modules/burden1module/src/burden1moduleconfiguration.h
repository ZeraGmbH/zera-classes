#ifndef BURDEN1MODULECONFIGURATION_H
#define BURDEN1MODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace BURDEN1MODULE
{

enum moduleconfigstate
{
    setDebugLevel,
    setInputModule,
    setSystemCount,
    setSystemUnit,

    setNominalRange,
    setNominalRangeFactor,
    setNominalBurden,
    setWireLength,
    setWireCrosssection,

    setMeasSystem1 = 16, // we leave some place for additional cmds

    setnext = setMeasSystem1 + 16
};



class cBurden1ModuleConfigData;

const QString defaultXSDFile = "://src/burden1module.xsd";

// moduleconfiguration holds configuration data as well as parameter

class cBurden1ModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cBurden1ModuleConfiguration();
    ~cBurden1ModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cBurden1ModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cBurden1ModuleConfigData *m_pBurden1ModulConfigData;  // configuration
};

}

#endif // BURDEN1MODULCONFIGURATION_H
