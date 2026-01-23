#ifndef BURDEN1MODULEMEASPROGRAM_H
#define BURDEN1MODULEMEASPROGRAM_H

#include "burden1moduleconfigdata.h"
#include "burden1measdelegate.h"
#include <basemeasworkprogram.h>
#include <vfmoduleparameter.h>
#include <vfmodulemetadata.h>
#include <QFinalState>
#include <vs_abstractcomponent.h>

namespace BURDEN1MODULE
{

class cBurden1Module;

class cBurden1ModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT

public:
    cBurden1ModuleMeasProgram(cBurden1Module* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;
public slots:
    virtual void start() override {}; // follow DFT
    virtual void stop() override {};

private:
    cBurden1ModuleConfigData* getConfData();
    void setParameters();

    cBurden1Module* m_pModule;
    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pBRSCountInfo; // the number of Burden system we are configured for
    VfModuleComponent* m_pMeasureSignal;

    VfModuleParameter* m_pNominalRangeParameter;
    VfModuleParameter* m_pNominalRangeFactorParameter;
    VfModuleParameter* m_pNominalBurdenParameter;
    VfModuleParameter* m_pWireLengthParameter;
    VfModuleParameter* m_pWireCrosssectionParameter;
    VeinStorage::AbstractComponentPtr m_dftSignal;

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

    void onDftSigChange(const QVariant &value);
    void newNominalRange(const QVariant &nr);
    void newNominalFactorRange(const QVariant &nrf);
    void newNominalBurden(const QVariant &nb);
    void newWireLength(const QVariant &wl);
    void newWireCrosssection(const QVariant &wc);
};

}
#endif // Burden1MODULEMEASPROGRAM_H
