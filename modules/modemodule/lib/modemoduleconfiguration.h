#ifndef MODEMODULECONFIGURATION_H
#define MODEMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace MODEMODULE
{

enum moduleconfigstate
{
    setMeasMode,
    setSamplingChnNr,
    setSamplingSigPeriod,
    setSamplingMeasPeriod
};



class cModeModuleConfigData;

const QString defaultXSDFile = "://modemodule.xsd";

// modemoduleconfiguration holds configuration data as well as parameter

class cModeModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cModeModuleConfiguration();
    ~cModeModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cModeModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cModeModuleConfigData *m_pModeModulConfigData;  // configuration
};

}

#endif // MODEMODULCONFIGURATION_H
