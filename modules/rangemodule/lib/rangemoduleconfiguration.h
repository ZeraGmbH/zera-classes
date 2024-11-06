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
    setSession,
    setChannelCount,
    setSubdcCount,
    setGroupCount,
    setGrouping,
    setRangeAutomatic,
    setMeasureInterval,
    setAdjustInterval,
    setAdjustIgnoreRmsValuesEnable,
    setAdjustIgnoreRmsValuesThreshold,

    setDefaultRange1 = 40, // max. 32 channels
    setSenseChannel1 = 72, // dito
    setSubdcChannel1 = 104, // dito
    setExtendChannel1 = 136, // dito
    setInvertChannel1 = 168, // dito

    setGroup1ChannelCount = 222, // max. 10 groups
    setGroup1Channel1 = 200
};



class cRangeModuleConfigData;

const QString defaultXSDFile = "://rangemodule.xsd";

// rangemoduleconfiguration holds configuration data as well as parameter

class cRangeModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cRangeModuleConfiguration();
    ~cRangeModuleConfiguration();
    void setConfiguration(QByteArray xmlString) override;
    virtual QByteArray exportConfiguration() override; // exports conf. and parameters to xml
    cRangeModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key) override;
    virtual void completeConfiguration(bool ok);

private:
    cRangeModuleConfigData *m_pRangeModulConfigData;  // configuration
};

}

#endif // RANGEMODULCONFIGURATION_H
