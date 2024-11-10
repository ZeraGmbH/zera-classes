#ifndef POWER3MODULEMEASPROGRAM_H
#define POWER3MODULEMEASPROGRAM_H

#include "power3moduleconfigdata.h"
#include "power3measdelegate.h"
#include <basemeasworkprogram.h>
#include <vfmoduleactvalue.h>
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
    virtual ~cPower3ModuleMeasProgram();
    void generateInterface() override;

public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop

private:
    cPower3ModuleConfigData* getConfData();

    cPower3Module* m_pModule;
    QList<VfModuleActvalue*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pHPWCountInfo; // the number of values we produce
    VfModuleComponent* m_pMeasureSignal;

    QList<cPower3MeasDelegate*> m_Power3MeasDelegateList;

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
#endif // POWER3MODULEMEASPROGRAM_H
