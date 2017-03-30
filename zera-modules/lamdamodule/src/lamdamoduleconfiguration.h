#ifndef LAMDAMODULECONFIGURATION_H
#define LAMDAMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace LAMDAMODULE
{

enum moduleconfigstate
{
    setDebugLevel,
    setSystemCount,

    setLamdaInputPEntity1 = 16, // we leave some place for additional cmds
    setLamdaInputSEntity1 =setLamdaInputPEntity1 +16,
    setLamdaInputPComponent1 = setLamdaInputSEntity1 + 16,
    setLamdaInputSComponent1 = setLamdaInputPComponent1 +16
};



class cLamdaModuleConfigData;

const QString defaultXSDFile = "://src/lamdamodule.xsd";

// moduleconfiguration holds configuration data as well as parameter

class cLamdaModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cLamdaModuleConfiguration();
    ~cLamdaModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cLamdaModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cLamdaModuleConfigData *m_pLamdaModulConfigData;  // configuration
};

}

#endif // LAMDAMODULCONFIGURATION_H
