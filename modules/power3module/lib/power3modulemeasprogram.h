#ifndef POWER3MODULEMEASPROGRAM_H
#define POWER3MODULEMEASPROGRAM_H

#include "power3moduleconfigdata.h"
#include "power3measdelegate.h"
#include <basemeasworkprogram.h>
#include <vfmodulemetadata.h>
#include <QFinalState>
#include <memory>

namespace POWER3MODULE
{

class cPower3Module;

class cPower3ModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT

public:
    cPower3ModuleMeasProgram(cPower3Module* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;

public slots:
    void start() override {}; // follow FFT
    void stop() override {};

private:
    cPower3ModuleConfigData* getConfData();

    cPower3Module* m_pModule;
    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pHPWCountInfo; // the number of values we produce
    VfModuleComponent* m_pMeasureSignal;
    VeinStorage::AbstractComponentPtr m_fftSignal;

    QList<cPower3MeasDelegate*> m_Power3MeasDelegateList;

    // statemachine for activating gets the following states
    QState m_searchActualValuesState;
    QFinalState m_activationDoneState;

    // statemachine for deactivating
    QState m_deactivateState;
    QFinalState m_deactivateDoneState;

private slots:
    void searchActualValues();
    void onFftSigChange(const QVariant &value);
    void activateDone();

    void deactivateMeas();
    void deactivateMeasDone();
};

}
#endif // POWER3MODULEMEASPROGRAM_H
