#ifndef REFERENCEMODULECONFIGURATION_H
#define REFERENCEMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace REFERENCEMODULE
{

enum moduleconfigstate
{
    setRMIp,
    setRMPort,
    setPCBServerIp,
    setPCBServerPort,
    setDSPServerIp,
    setDSPServerPort,
    setChannelCount,
    setMeasureInterval,
    setMeasureIgnore,

    setReferenceChannel1 = 16 // max. 32 channels

};



class cReferenceModuleConfigData;

// referencemoduleconfiguration holds configuration data as well as parameter

class cReferenceModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cReferenceModuleConfiguration();
    ~cReferenceModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cReferenceModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cReferenceModuleConfigData *m_pReferenceModulConfigData;  // configuration
};

}

#endif // REFERENCEMODULCONFIGURATION_H
