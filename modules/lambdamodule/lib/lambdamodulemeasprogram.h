#ifndef LAMBDAMODULEMEASPROGRAM_H
#define LAMBDAMODULEMEASPROGRAM_H

#include "lambdamoduleconfigdata.h"
#include "lambdacalcdelegate.h"
#include "basemeasworkprogram.h"
#include "vfmodulecomponent.h"
#include "vfmodulemetadata.h"
#include <QList>
#include <QHash>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace LAMBDAMODULE
{
class cLambdaModule;

class cLambdaModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT
public:
    cLambdaModuleMeasProgram(cLambdaModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual void generateInterface();
public slots:
    virtual void start() {}
    virtual void stop() {}

private:
    cLambdaModuleConfigData* getConfData();

    cLambdaModule* m_pModule;
    QList<VfModuleActvalue*> m_veinLambdaActValues;
    QList<VfModuleActvalue*> m_veinLoadTypeList;
    VfModuleMetaData* m_pLAMBDACountInfo; // the number of values we produce
    VfModuleComponent* m_pMeasureSignal;

    LambdaCalcDelegate *m_lambdaCalcDelegate;
    // statemachine for activating gets the following states
    QState m_searchActualValuesState;
    QFinalState m_activationDoneState;

    // statemachine for deactivating
    QState m_deactivateState;
    QFinalState m_deactivateDoneState;

private slots:
    void searchActualValues();
    void activateDone();

    void deactivateMeas();
    void deactivateMeasDone();

    void setMeasureSignal(int signal);
};

}
#endif // LAMBDAMODULEMEASPROGRAM_H
