#ifndef THDNMODULECONFIGURATION_H
#define THDNMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace THDNMODULE
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
    setValueCount,
    setMeasureInterval,
    setMovingwindowBool,
    setMovingwindowTime,

    setValue1 = 20
};



class cThdnModuleConfigData;

//const QString defaultXSDFile = "/etc/zera/modules/thdnmodule.xsd";
const QString defaultXSDFile = "/home/peter/C++/zera-classes/zera-modules/thdnmodule/src/thdnmodule.xsd";

// rangemoduleconfiguration holds configuration data as well as parameter

class cThdnModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cThdnModuleConfiguration();
    ~cThdnModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cThdnModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cThdnModuleConfigData *m_pThdnModulConfigData;  // configuration
};

}

#endif // THDNMODULCONFIGURATION_H
