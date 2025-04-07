#ifndef SFCMODULEMEASPROGRAM_H
#define SFCMODULEMEASPROGRAM_H

#include "sfcmoduleconfigdata.h"
#include "basemeasworkprogram.h"
#include "vfmoduleactvalue.h"
#include "vfmoduleparameter.h"
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace SFCMODULE
{

enum sfcmoduleCmds
{
};

class cSfcModule;
class cSfcModuleMeasProgram : public cBaseMeasWorkProgram
{
    Q_OBJECT
public:
    cSfcModuleMeasProgram(cSfcModule *module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    virtual ~cSfcModuleMeasProgram();
    void generateVeinInterface() override; // here we export our interface (entities)

public slots:
    void start() override; // difference between start and stop is that actual values
    void stop() override;  // in interface are not updated when stop

private:
    cSfcModuleConfigData *getConfData();

    cSfcModule *m_pModule;
    QList<VfModuleActvalue *> m_veinActValueList; // the list of actual values we work on

    // statemachine for activating gets the following states
    QFinalState m_activationDoneState;

    // statemachine for deactivating
    QFinalState m_deactivateDoneState;

    VfModuleParameter* m_pStartStop;

private slots:
    void activateDone();
    void deactivateMeasDone();
    void onStartStopChanged();
};

}
#endif // SFCMODULEMEASPROGRAM_H
