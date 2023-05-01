#ifndef RMSMODULECONFIGURATION_H
#define RMSMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace RMSMODULE
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
    setIntegrationMode,
    setMovingwindowBool,
    setMovingwindowTime,
    setMeasureIntervalTime,
    setMeasureIntervalPeriod,

    setValue1 = 20
};



class cRmsModuleConfigData;

// rangemoduleconfiguration holds configuration data as well as parameter

class cRmsModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cRmsModuleConfiguration();
    ~cRmsModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cRmsModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cRmsModuleConfigData *m_pRmsModulConfigData;  // configuration
};

}

#endif // RMSMODULCONFIGURATION_H
