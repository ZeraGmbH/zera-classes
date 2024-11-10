#ifndef TRANSFORMER1MODULEMEASPROGRAM_H
#define TRANSFORMER1MODULEMEASPROGRAM_H

#include "transformer1moduleconfigdata.h"
#include "basemeasworkprogram.h"
#include <QList>
#include <QFinalState>

class VfModuleActvalue;
class VfModuleMetaData;
class VfModuleComponent;
class VfModuleParameter;

namespace TRANSFORMER1MODULE
{

class cTransformer1ModuleConfigData;
class cTransformer1MeasDelegate;
class cTransformer1Module;

class cTransformer1ModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT

public:
    cTransformer1ModuleMeasProgram(cTransformer1Module* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    virtual ~cTransformer1ModuleMeasProgram();
    void generateInterface() override;

public slots:
    void start() override;
    void stop() override;

private:
    cTransformer1ModuleConfigData* getConfData();

    cTransformer1Module* m_pModule;
    QList<VfModuleActvalue*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pTRSCountInfo; // the number of transformer system we are configured for
    VfModuleComponent* m_pMeasureSignal;

    VfModuleParameter* m_pPrimClampPrimParameter;
    VfModuleParameter* m_pPrimClampSecParameter;
    VfModuleParameter* m_pSecClampPrimParameter;
    VfModuleParameter* m_pSecClampSecParameter;
    VfModuleParameter* m_pPrimDutParameter;
    VfModuleParameter* m_pSecDutParameter;

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
