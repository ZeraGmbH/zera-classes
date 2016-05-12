#ifndef RANGEMODULECONFIGURATION_H
#define RANGEMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include <basemoduleconfiguration.h>

namespace RANGEMODULE
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
    setChannelCount,
    setSubdcCount,
    setGroupCount,
    setGrouping,
    setRangeAutomatic,
    setMeasureInterval,
    setAdjustInterval,
    setGroupingBool,
    setRangeAutomaticBool,
    setOverloadBool,

    setDefaultRange1 = 40, // max. 32 channels
    setSenseChannel1 = 72, // max. 32 channels
    setSubdcChannel1 = 104,
    setGroup1ChannelCount = 136, // max. 10 groups
    setGroup1Channel1 = 146
};



class cRangeModuleConfigData;

const QString defaultXSDFile = "://src/rangemodule.xsd";

// rangemoduleconfiguration holds configuration data as well as parameter

class cRangeModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cRangeModuleConfiguration();
    ~cRangeModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cRangeModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cRangeModuleConfigData *m_pRangeModulConfigData;  // configuration
};

}

#endif // RANGEMODULCONFIGURATION_H
