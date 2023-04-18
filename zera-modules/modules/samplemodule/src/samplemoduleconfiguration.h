#ifndef SAMPLEMODULECONFIGURATION_H
#define SAMPLEMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace SAMPLEMODULE
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
    setPllAutomaticBool,
    setPllAutomatic,
    setMeasureInterval,
    setSampleSystem, // we only support 1 system
    setPllChannelCount, // the number of sense channels the pll can be set to
    setSampleSystemRange,

    setPllChannel1 = 16, // max. 32 channels
    setPllChannel = 48, // the pll setting we start with
};



class cSampleModuleConfigData;

const QString defaultXSDFile = "://src/samplemodule.xsd";

// samplemoduleconfiguration holds configuration data as well as parameter

class cSampleModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cSampleModuleConfiguration();
    ~cSampleModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cSampleModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cSampleModuleConfigData *m_pSampleModulConfigData;  // configuration
};

}

#endif // SAMPLEMODULCONFIGURATION_H
