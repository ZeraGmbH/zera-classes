#ifndef RANGEMODULECONFIGURATION_H
#define RANGEMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

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
    setGroupCount,
    setGrouping,
    setAutomatic,
    setInterval,

    setSenseChannel1 = 20, // max. 32 channels
    setGroup1ChannelCount = 52, // max. 10 groups
    setGroup1Channel1 = 62
};



class cRangeModuleConfigData;

// const QString defaultXSDFile = "/etc/zera/module/rangemodule.xsd";
const QString defaultXSDFile = "/home/peter/C++/rangemodule/src/rangemodule.xsd";

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
    // QHash<QString, VeinEntity*> m_exportEntityList; // we hold a list of all our entities for export
    cRangeModuleConfigData *m_pRangeModulConfigData;  // configuration
};

#endif // RANGEMODULCONFIGURATION_H
