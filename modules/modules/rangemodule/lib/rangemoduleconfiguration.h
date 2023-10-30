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
    setSession,
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

    setDefaultRange1 = 40, // max. 32 channels
    setSenseChannel1 = 72, // dito
    setSubdcChannel1 = 104, // dito
    setExtendChannel1 = 136, // dito

    setGroup1ChannelCount = 168, // max. 10 groups
    setGroup1Channel1 = 146
};



class cRangeModuleConfigData;

const QString defaultXSDFile = "://rangemodule.xsd";

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
