#ifndef TRANSFORMER1MODULEMEASPROGRAM_H
#define TRANSFORMER1MODULEMEASPROGRAM_H

#include "basemeasworkprogram.h"
#include "transformer1measdelegate.h"
#include "vfmodulemetadata.h"
#include "vfmoduleparameter.h"
#include <QList>
#include <QFinalState>

namespace TRANSFORMER1MODULE
{

class cTransformer1Module;

class cTransformer1ModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT
public:
    explicit cTransformer1ModuleMeasProgram(cTransformer1Module* module);
    void generateVeinInterface() override;

public slots:
    void start() override;
    void stop() override;

private:
    cTransformer1Module* m_pModule = nullptr;
    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pTRSCountInfo = nullptr; // the number of transformer system we are configured for
    VfModuleComponent* m_pMeasureSignal = nullptr;

    VfModuleParameter* m_pPrimClampPrimParameter = nullptr;
    VfModuleParameter* m_pPrimClampSecParameter = nullptr;
    VfModuleParameter* m_pSecClampPrimParameter = nullptr;
    VfModuleParameter* m_pSecClampSecParameter = nullptr;
    VfModuleParameter* m_pPrimDutParameter = nullptr;
    VfModuleParameter* m_pSecDutParameter = nullptr;

    QList<cTransformer1MeasDelegate*> m_Transformer1MeasDelegateList;

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

    void newPrimClampPrim(QVariant pcp);
    void newPrimClampSec(QVariant pcs);
    void newSecClampPrim(QVariant scp);
    void newSecClampSec(QVariant scs);
    void newPrimDut(QVariant pd);
    void newSecDut(QVariant sd);

    void setParameters();
};

}
#endif // TRANSFORMER1MODULEMEASPROGRAM_H
