#ifndef LAMBDAMODULECONFIGURATION_H
#define LAMBDAMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace LAMBDAMODULE
{

enum moduleconfigstate
{
    setDebugLevel,
    setSystemCount,
    setActivePMeasModeAvail,
    setActivePMeasModeEntity,
    setActivePMeasModeComponent,
    setActivePMeasModePhaseSelectComponent,

    setLambdaInputPEntity1 = 16, // we leave some place for additional cmds
    setLambdaInputQEntity1 =setLambdaInputPEntity1 +16,
    setLambdaInputSEntity1 =setLambdaInputQEntity1 +16,
    setLambdaInputPComponent1 = setLambdaInputSEntity1 + 16,
    setLambdaInputQComponent1 = setLambdaInputPComponent1 +16,
    setLambdaInputSComponent1 = setLambdaInputQComponent1 +16
};



class cLambdaModuleConfigData;

// moduleconfiguration holds configuration data as well as parameter

class cLambdaModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cLambdaModuleConfiguration();
    ~cLambdaModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cLambdaModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cLambdaModuleConfigData *m_pLambdaModulConfigData;  // configuration
};

}

#endif // LAMBDAMODULCONFIGURATION_H
