#ifndef DOSAGEMODULEMEASPROGRAM_H
#define DOSAGEMODULEMEASPROGRAM_H

#include "dosagemoduleconfigdata.h"
#include "basemeasworkprogram.h"
//#include "vfmoduleactvalue.h"
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace DOSAGEMODULE
{

enum dosagemoduleCmds
{
};

class cDosageModule;
class cDosageModuleMeasProgram : public cBaseMeasWorkProgram
{
    Q_OBJECT
public:
    cDosageModuleMeasProgram(cDosageModule *module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    virtual ~cDosageModuleMeasProgram();
    void generateVeinInterface() override; // here we export our interface (entities)

public slots:
    void start() override; // difference between start and stop is that actual values
    void stop() override;  // in interface are not updated when stop

private:
    cDosageModuleConfigData *getConfData();

    cDosageModule *m_pModule;
//    QList<VfModuleActvalue *> m_veinActValueList; // the list of actual values we work on

    // statemachine for activating gets the following states
    QFinalState m_activationDoneState;

    // statemachine for deactivating
    QFinalState m_deactivateDoneState;

private slots:
    void activateDone();
    void deactivateMeasDone();
};

}
#endif // DOSAGEMODULEMEASPROGRAM_H
