#ifndef TRANSFORMER1MODULECONFIGURATION_H
#define TRANSFORMER1MODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace TRANSFORMER1MODULE
{

enum moduleconfigstate
{
    setDebugLevel,
    setInputModule,
    setSystemCount,
    setSystemUnit,

    setPrimClampPrim,
    setPrimClampSec,
    setSecClampPrim,
    setSecClampSec,
    setPrimDut,
    setSecDut,

    setMeasSystem1 = 16, // we leave some place for additional cmds

    setnext = setMeasSystem1 + 16
};



class cTransformer1ModuleConfigData;

const QString defaultXSDFile = "://transformer1module.xsd";

// moduleconfiguration holds configuration data as well as parameter

class cTransformer1ModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cTransformer1ModuleConfiguration();
    ~cTransformer1ModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cTransformer1ModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cTransformer1ModuleConfigData *m_pTransformer1ModulConfigData;  // configuration
};

}

#endif // TRANSFORMER1MODULCONFIGURATION_H
