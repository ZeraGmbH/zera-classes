#ifndef BLEMODULEMEASPROGRAM_H
#define BLEMODULEMEASPROGRAM_H

#include "blemoduleconfigdata.h"
#include "basemeasworkprogram.h"
#include "vfmoduleactvalue.h"
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace BLEMODULE
{

enum blemoduleCmds
{
};

class cBleModule;
class cBleModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT
public:
    cBleModuleMeasProgram(cBleModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cBleModuleMeasProgram();
    void generateInterface() override; // here we export our interface (entities)

public slots:
    void start() override; // difference between start and stop is that actual values
    void stop() override; // in interface are not updated when stop

private:
    cBleModuleConfigData* getConfData();

    cBleModule* m_pModule;
    QList<VfModuleActvalue*> m_veinActValueList; // the list of actual values we work on

    // statemachine for activating gets the following states
    QFinalState m_activationDoneState;

    // statemachine for deactivating
    QFinalState m_deactivateDoneState;

private slots:
    void activateDone();
    void deactivateMeasDone();
};

}
#endif // BLEMODULEMEASPROGRAM_H
