#ifndef BURDEN1MODULEMEASPROGRAM_H
#define BURDEN1MODULEMEASPROGRAM_H

#include "burden1moduleconfigdata.h"
#include "burden1measdelegate.h"
#include <basemeasworkprogram.h>
#include <vfmoduleparameter.h>
#include <vfmodulemetadata.h>
#include <QFinalState>

namespace BURDEN1MODULE
{

class cBurden1Module;

class cBurden1ModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT

public:
    cBurden1ModuleMeasProgram(cBurden1Module* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    virtual ~cBurden1ModuleMeasProgram();
    void generateInterface() override;
public slots:
    virtual void start() override {};
    virtual void stop() override {};

private:
    cBurden1ModuleConfigData* getConfData();

    cBurden1Module* m_pModule;
    QList<VfModuleActvalue*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pBRSCountInfo; // the number of Burden system we are configured for
    VfModuleComponent* m_pMeasureSignal;

    VfModuleParameter* m_pNominalRangeParameter;
    VfModuleParameter* m_pNominalRangeFactorParameter;
    VfModuleParameter* m_pNominalBurdenParameter;
    VfModuleParameter* m_pWireLengthParameter;
    VfModuleParameter* m_pWireCrosssectionParameter;

    QList<cBurden1MeasDelegate*> m_Burden1MeasDelegateList;

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

    void newNominalRange(QVariant nr);
    void newNominalFactorRange(QVariant nrf);
    void newNominalBurden(QVariant nb);
    void newWireLength(QVariant wl);
    void newWireCrosssection(QVariant wc);
    void setParameters();
};

}
#endif // Burden1MODULEMEASPROGRAM_H
