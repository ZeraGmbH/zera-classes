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
    explicit cLambdaModuleMeasProgram(cLambdaModule* module);
    void generateVeinInterface() override;
public slots:
    void start() override {}
    void stop() override {}

private:
    cLambdaModule* m_pModule = nullptr;
    QList<VfModuleComponent*> m_veinLambdaActValues;
    QList<VfModuleComponent*> m_veinLoadTypeList;
    VfModuleMetaData* m_pLAMBDACountInfo = nullptr; // the number of values we produce
    VfModuleComponent* m_pMeasureSignal = nullptr;

    LambdaCalcDelegate *m_lambdaCalcDelegate = nullptr;
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
