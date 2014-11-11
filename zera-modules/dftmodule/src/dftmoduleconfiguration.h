#ifndef DFTMODULECONFIGURATION_H
#define DFTMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace DFTMODULE
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
    setDftOrder,
    setValueCount,
    setMeasureInterval,
    setMovingwindowBool,
    setMovingwindowTime,

    setValue1 = 20
};



class cDftModuleConfigData;

//const QString defaultXSDFile = "/etc/zera/modules/dftmodule.xsd";
const QString defaultXSDFile = "/home/peter/C++/zera-classes/zera-modules/dftmodule/src/dftmodule.xsd";

// rangemoduleconfiguration holds configuration data as well as parameter

class cDftModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cDftModuleConfiguration();
    ~cDftModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cDftModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cDftModuleConfigData *m_pDftModulConfigData;  // configuration
};

}

#endif // DFTMODULCONFIGURATION_H
