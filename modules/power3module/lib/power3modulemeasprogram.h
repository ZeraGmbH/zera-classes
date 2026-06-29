#ifndef POWER3MODULEMEASPROGRAM_H
#define POWER3MODULEMEASPROGRAM_H

#include "power3measdelegate.h"
#include <basemeasworkprogram.h>
#include <vfmodulemetadata.h>
#include <QFinalState>

namespace POWER3MODULE
{

class cPower3Module;

class cPower3ModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT
public:
    explicit cPower3ModuleMeasProgram(cPower3Module* module);
    void generateVeinInterface() override;

public slots:
    void start() override {}; // follow FFT
    void stop() override {};

private:
    cPower3Module* m_pModule = nullptr;
    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pHPWCountInfo = nullptr; // the number of values we produce
    VfModuleComponent* m_pMeasureSignal = nullptr;
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
