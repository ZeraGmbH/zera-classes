#ifndef LAMBDAMODULECONFIGURATION_H
#define LAMBDAMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "lambdamoduleconfigdata.h"
#include <QByteArray>

namespace LAMBDAMODULE
{

enum moduleconfigstate
{
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

class cLambdaModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cLambdaModuleConfiguration();
    ~cLambdaModuleConfiguration();
    void setConfiguration(const QByteArray& xmlString) override;
    QByteArray exportConfiguration() override; // exports conf. and parameters to xml
    cLambdaModuleConfigData* getConfigurationData();

protected slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    cLambdaModuleConfigData *m_pLambdaModulConfigData = nullptr;  // configuration
};

}

#endif // LAMBDAMODULCONFIGURATION_H
